@file:OptIn(ExperimentalUnsignedTypes::class)

package com.example.dc_acconverterandcontrolremote

import android.Manifest
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.content.pm.PackageManager
import android.net.ConnectivityManager
import android.net.LinkProperties
import android.net.Network
import android.net.NetworkCapabilities
import android.net.NetworkRequest
import android.net.wifi.aware.AttachCallback
import android.net.wifi.aware.Characteristics.WIFI_AWARE_CIPHER_SUITE_NCS_PK_128
import android.net.wifi.aware.DiscoverySessionCallback
import android.net.wifi.aware.IdentityChangedListener
import android.net.wifi.aware.PeerHandle
import android.net.wifi.aware.PublishConfig
import android.net.wifi.aware.SubscribeConfig
import android.net.wifi.aware.SubscribeDiscoverySession
import android.net.wifi.aware.WifiAwareDataPathSecurityConfig
import android.net.wifi.aware.WifiAwareManager
import android.net.wifi.aware.WifiAwareNetworkInfo
import android.net.wifi.aware.WifiAwareNetworkSpecifier
import android.net.wifi.aware.WifiAwareSession
import android.os.Build
import android.widget.Toast
import androidx.annotation.RequiresApi
import androidx.annotation.RequiresPermission
import androidx.core.app.ActivityCompat
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.coroutineScope
import kotlinx.coroutines.launch
import java.io.DataOutputStream
import java.io.InputStream
import java.io.OutputStream
import java.net.Inet6Address
import java.net.InetAddress
import java.net.NetworkInterface
import java.net.Socket
import java.net.SocketException
import java.util.Enumeration
import kotlin.ByteArray
import kotlinx.coroutines.sync.Mutex
import kotlinx.coroutines.sync.withLock
import kotlinx.coroutines.withContext
import androidx.compose.runtime.rememberCoroutineScope
import kotlinx.coroutines.CompletableDeferred
import kotlinx.coroutines.CoroutineScope

const val HANDSHAKE_OK = 0XFF
const val  HANDSHAKE_FAILED= 0X00

//macro to identify received data for scheduler devices
const val  RECEIVED_SCHEDULER= 1

//macro to identify data for run control remote
const val RECEIVED_C_REMOTE = 2

//macro to define data for setup time
const val RECEIVED_TIME = 4

//macro to define data for schedule charger time
const val RECEIVED_CHARGER_TIME= 8

//macro to identify request present esp32 configuration feed back
const val RECEIVED_REPORT_REQ= 16

//macro to define new matchfilter
const val RECEIVED_MATCH_FILTER= 32

//macro to define quantity of devices
const val RECEIVED_QUANTITY_DEVICES= 64


@OptIn(ExperimentalUnsignedTypes::class)
class WifiAware(val context: Context, val viewModel: DeviceSchedulerViewModel) {


    private val MAC_ADDRESS_MESSAGE: Int = 55
    private val IP_ADDRESS_MESSAGE = 33

    var networkSpecifier: WifiAwareNetworkSpecifier? = null

    var config: PublishConfig? = null

    var wifiAwareSession: WifiAwareSession? = null

    var wifiAwareManager: WifiAwareManager? = null

    var secConfig: WifiAwareDataPathSecurityConfig? = null

    var subscribeConfig: SubscribeConfig? = null

    var subscribeDiscoverySession: SubscribeDiscoverySession? = null

    var peerHandle: PeerHandle? = null

    var portToUse: Int? = null

    var connectivityManager: ConnectivityManager? = null

    var socket: Socket? = null

    // pending definitions
    var serviceSpecificInfo: ByteArray? = null

    var matchFilter: List<ByteArray> = listOf(viewModel.getMatchFilterLaunch())


    var flagReceived: Boolean=false

    private val wifiMutex = Mutex()

    var currentNetwork: Network? = null

    @RequiresApi(Build.VERSION_CODES.O)
    @RequiresPermission(Manifest.permission.ACCESS_WIFI_STATE)
    fun attachToWifi(deferred: CompletableDeferred<Boolean>? = null) {

        if (wifiAwareSession != null) {
            deferred?.complete(true) // Already attached
            return
        }

        if (wifiAwareManager == null || !wifiAwareManager!!.isAvailable()) {

            Toast.makeText(context, R.string.connectionError, Toast.LENGTH_SHORT).show()

            println("wifi unavailable")
            return;
        }

        if (ActivityCompat.checkSelfPermission(
                context,
                Manifest.permission.ACCESS_FINE_LOCATION
            ) != PackageManager.PERMISSION_GRANTED || ActivityCompat.checkSelfPermission(
                context,
                Manifest.permission.NEARBY_WIFI_DEVICES
            ) != PackageManager.PERMISSION_GRANTED
        ) {

        }
        wifiAwareManager!!.attach(object : AttachCallback() {
            override fun onAttached(session: WifiAwareSession) {
                super.onAttached(session)
                wifiAwareSession = session;
                println("wifi session successful")
                deferred?.complete(true)
            }

            override fun onAttachFailed() {
                super.onAttachFailed();
                wifiAwareSession!!.close()
                wifiAwareSession = null
                println("Aware session failed")
                Toast.makeText(context, R.string.connectionError, Toast.LENGTH_SHORT).show()
                deferred?.complete(false)
            }

        }, object : IdentityChangedListener() {
            @Override
            override fun onIdentityChanged(mac: ByteArray) {
                super.onIdentityChanged(mac)
                // In this program mac address remote should be set only one time
                viewModel.MacSetLaunchRemote(viewModel.setMacAddressToString(mac))
                println("New mac Address had been set ")
            }
        }, null);
    }

    @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
    fun requestWifiNetwork() {
        if (networkSpecifier == null) {
            return;
        }

        val networkRequest: NetworkRequest = NetworkRequest.Builder()
            .addTransportType(NetworkCapabilities.TRANSPORT_WIFI_AWARE)
            .setNetworkSpecifier(networkSpecifier)
            .build()

        connectivityManager!!.requestNetwork(
            networkRequest,
            object : ConnectivityManager.NetworkCallback() {

                override fun onAvailable(network: Network) {
                    super.onAvailable(network);
                    currentNetwork = network
                }

                override fun onLosing(network: Network, maxMsToLive: Int) {
                    super.onLosing(network, maxMsToLive)
                    Toast.makeText(context, R.string.connectionError, Toast.LENGTH_SHORT).show()

                }

                override fun onLost(network: Network) {
                    super.onLost(network)
                    Toast.makeText(context, R.string.connectionError, Toast.LENGTH_SHORT).show()

                }

                override fun onUnavailable() {
                    super.onUnavailable()
                    Toast.makeText(context, R.string.connectionError, Toast.LENGTH_SHORT).show()

                }

                override fun onCapabilitiesChanged(network: Network, networkCapabilities: NetworkCapabilities) {
                    super.onCapabilitiesChanged(network, networkCapabilities)

                    // 1. Get the transport info specifically as WifiAwareNetworkInfo
                    val peerAwareInfo = networkCapabilities.transportInfo as? WifiAwareNetworkInfo ?: return

                    // 2. Capture the port assigned by the ESP32 (which should be 8080)
                    val discoveredPort = peerAwareInfo.port

                    // 3. Update your ViewModel so sendData() knows to use 8080
                    viewModel.peerPortSetLaunch(discoveredPort)

                    println("Verified: Connecting to ESP32 on Port $discoveredPort")
                }

                override fun onLinkPropertiesChanged(
                    network: Network,
                    linkProperties: LinkProperties
                ) {
                    super.onLinkPropertiesChanged(network, linkProperties);
                    //TODO: create socketServer on different thread to transfer files
                    // this program only use one Thread, valid for other usages
                    try {

                        val awareNi: NetworkInterface = NetworkInterface.getByName(
                            linkProperties.getInterfaceName()
                        )

                        val Addresses: Enumeration<InetAddress> = awareNi.getInetAddresses();

                        while (Addresses.hasMoreElements()) {
                            var addr: InetAddress = Addresses.nextElement();
                            if (addr is Inet6Address) {

                                if (addr.isLinkLocalAddress()) {

                                    viewModel.IpSetLaunchLocal(
                                        viewModel.setIpAddressToString(
                                            Inet6Address.getByAddress(
                                                "WifiAware",
                                                addr.getAddress(),
                                                awareNi
                                            ).address
                                        ), context
                                    )
                                    //    this application will not use publish
                                    //   if (publishDiscoverySession != null && peerHandle != null) {
                                    //       publishDiscoverySession.sendMessage(peerHandle, IP_ADDRESS_MESSAGE, myIP);
                                    if (subscribeDiscoverySession != null && peerHandle != null) {
                                        subscribeDiscoverySession!!.sendMessage(
                                            peerHandle!!, IP_ADDRESS_MESSAGE,
                                            viewModel.setIpStringToAddress(viewModel.getIpAddressLocalLaunch()!!)
                                        );

                                    }
                                    break;
                                }
                            }
                        }
                    } catch (e: SocketException) {
                        Toast.makeText(context, R.string.connectionError, Toast.LENGTH_SHORT).show()
                        println("onlinkpropertychanged error socket $e")
                    } catch (e: Exception) {

                        Toast.makeText(context, R.string.connectionError, Toast.LENGTH_SHORT).show()
                        println("onlinkpropertychanged error socket $e")
                    }
                }
            }
        )
    }


    @RequiresApi(Build.VERSION_CODES.TIRAMISU)
    fun discover(): Intent? {
        if (viewModel.MATCH_FILTER.toString().length<7){
        viewModel.setMatchFilterLaunch("1234567\n")}

        if (context.packageManager.hasSystemFeature(PackageManager.FEATURE_WIFI_AWARE)) {


            wifiAwareManager =
                context.getSystemService(Context.WIFI_AWARE_SERVICE) as WifiAwareManager

            val filter = IntentFilter(WifiAwareManager.ACTION_WIFI_AWARE_STATE_CHANGED)

            val myReceiver = AwareBroadcastReceiver(context, viewModel, wifiAwareManager!!)

            return context.registerReceiver(myReceiver, filter)

        } else{
            Toast.makeText(context, R.string.connectionError, Toast.LENGTH_SHORT).show()
            return null
        }


    }

    // this function is included as reference but will not be used in this program Esp32 will be the publisher

    @RequiresApi(Build.VERSION_CODES.TIRAMISU)
    @RequiresPermission(allOf = [Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.NEARBY_WIFI_DEVICES])
    fun publish(pskOrPmk: String?, isPmk: Boolean, serviceName: String, serviceInfo: String) {
        // psk is 8 bits code as string
        // pmk is 32 characters alphanumeric code
        val pmkid = ByteArray(16)
        if (pskOrPmk != null) {
            if (isPmk) {
                secConfig
                WifiAwareDataPathSecurityConfig.Builder(
                    WIFI_AWARE_CIPHER_SUITE_NCS_PK_128
                )
                    .setPmk(pskOrPmk.toByteArray())
                    .setPmkId(pmkid)
                    .build()

            } else {
                secConfig = WifiAwareDataPathSecurityConfig.Builder(
                    WIFI_AWARE_CIPHER_SUITE_NCS_PK_128
                )
                    .setPskPassphrase(pskOrPmk)
                    .build();


            }
            config = PublishConfig.Builder()
                .setServiceName(serviceName)
                .setServiceSpecificInfo(serviceInfo.toByteArray())
                .setDataPathSecurityConfig(secConfig!!)
                .setPublishType(PublishConfig.PUBLISH_TYPE_SOLICITED)
                .build()


        } else {
            config = PublishConfig.Builder()
                .setServiceName(serviceName)
                .setServiceSpecificInfo(serviceInfo.toByteArray())
                .setPublishType(PublishConfig.PUBLISH_TYPE_SOLICITED)
                .build()

        }
    }

    @RequiresPermission(allOf = [Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.NEARBY_WIFI_DEVICES, Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.NEARBY_WIFI_DEVICES])
    @RequiresApi(Build.VERSION_CODES.TIRAMISU)
    fun subscribe() {


        subscribeConfig = SubscribeConfig.Builder()
            .setServiceName(viewModel.serviceName)
            .setServiceSpecificInfo(this@WifiAware.serviceSpecificInfo)
            .setMatchFilter(this@WifiAware.matchFilter)
            .build()

        if ((wifiAwareManager!!.isDeviceAttached) and (subscribeDiscoverySession == null)) {


            if (ActivityCompat.checkSelfPermission(
                    context,
                    Manifest.permission.ACCESS_FINE_LOCATION
                ) != PackageManager.PERMISSION_GRANTED || ActivityCompat.checkSelfPermission(
                    context,
                    Manifest.permission.NEARBY_WIFI_DEVICES
                ) != PackageManager.PERMISSION_GRANTED
            ) {
                Toast.makeText(context, R.string.permissionError, Toast.LENGTH_SHORT).show()

            }
            wifiAwareSession!!.subscribe(subscribeConfig!!, object : DiscoverySessionCallback() {

                override fun onServiceDiscovered(
                    peerHandle: PeerHandle,
                    serviceSpecificInfo: ByteArray,
                    matchFilter: List<ByteArray>
                ) {
                    // 1. Mandatory super call
                    super.onServiceDiscovered(peerHandle, serviceSpecificInfo, matchFilter)

                    // 2. Store the peerHandle for future communication
                    this@WifiAware.peerHandle = peerHandle

                    // 3. Build the Network Specifier for the Subscriber (Initiator)
                    // We use the discovery session and the peerHandle we just received
                    networkSpecifier = WifiAwareNetworkSpecifier.Builder(
                        subscribeDiscoverySession ?: return,
                        peerHandle
                    ).build()

                    // 4. NOW trigger the network tunnel creation
                    requestWifiNetwork()

                    // 5. Your existing logic to send the MAC address
                    if (subscribeDiscoverySession != null) {
                        subscribeDiscoverySession!!.sendMessage(
                            peerHandle, MAC_ADDRESS_MESSAGE,
                            viewModel.setMacStringToAddress(viewModel.getMacAddressLocalLaunch()!!)
                        )
                        println("onServiceDiscovered: Peer found, super called, requesting network...")
                    }
                }

                    override fun onSubscribeStarted(session: SubscribeDiscoverySession) {
                        super.onSubscribeStarted(session);

                        subscribeDiscoverySession = session;


                    }


                    override fun onMessageReceived(peerHandle: PeerHandle, message: ByteArray) {
                        super.onMessageReceived(peerHandle, message)
                        if (message.size == 2) {
                            portToUse = viewModel.setPortToUse(message);
                            println("received message, port = $portToUse")

                        } else if (message.size == 6) {

                            viewModel.MacSetLaunchRemote(
                                viewModel.setMacAddressToString(message))

                        } else if (message.size == 16) {;
                            viewModel.IpSetLaunchRemote(
                                viewModel.setIpAddressToString(message))

    // this program will not use other message after this will use socket
    //                    } else if (message.size > 16) {
    //                        viewModel.receivedMessage(message);
    //                        //Toast.makeText(MainActivity.this, "message received", Toast.LENGTH_SHORT).show();
                        }
                    }
             }, null)
            }
        }

    @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
    suspend fun sendData(macro: Int, data: ByteArray): Boolean = withContext(Dispatchers.IO) {
        // 1. Check if the network tunnel is ready
        val targetNetwork = currentNetwork ?: run {
            println("Error: currentNetwork is null. Tunnel not established yet.")

            Toast.makeText(context, R.string.connectionError, Toast.LENGTH_SHORT).show()

            return@withContext false
        }

        // Declare outside lock so finally block can see it
        var tempSocket: Socket? = null

        wifiMutex.withLock {
            try {
                // 2. Get connection details from ViewModel
                val port = viewModel.getPeerPortLaunch() ?: return@withContext false
                val ipv6String = viewModel.getIpAddressRemoteLaunch() ?: return@withContext false
                val address = Inet6Address.getByName(ipv6String)

                // 3. Create socket bound to the Aware Network
                tempSocket = targetNetwork.socketFactory.createSocket(address, port)

                // 4. Set explicit parameters to avoid compiler inference errors
                tempSocket?.soTimeout = 5000   // 5 second wait for ESP32 handshake
                tempSocket?.tcpNoDelay = true // Send immediately

                // 5. Send Macro and Data
                val output = DataOutputStream(tempSocket!!.getOutputStream())
                output.writeByte(macro)
                output.write(data)
                output.writeByte(HANDSHAKE_OK)
                output.flush()

                // 6. THE CHECK: Read handshake from ESP32
                val inputStream = tempSocket!!.getInputStream()
                val response = inputStream.read() // Waits for ESP32 response

                // Returns true only if ESP32 sends HANDSHAKE_OK (0xFF)
                return@withContext response == HANDSHAKE_OK

            } catch (e: Exception) {
                println("Socket Error: ${e.message}")
                return@withContext false
            } finally {
                // 7. Cleanup: Always close the socket
                try { tempSocket?.close() } catch (e: Exception) {

                    println("Error: Closing socket.")

                    Toast.makeText(context, R.string.connectionError, Toast.LENGTH_SHORT).show()

                }
            }
        }
    }

    @RequiresApi(Build.VERSION_CODES.TIRAMISU)
    suspend fun sendDataWithReturnData(macro: Int):ByteArray? {
        // 1. Check if the network tunnel is ready
        val targetNetwork = currentNetwork ?: run {
            println("Error: currentNetwork is null. Tunnel not established yet.")
            Toast.makeText(context, R.string.connectionError, Toast.LENGTH_SHORT).show()

            return null
        }

        // Declare outside lock so finally block can see it
        var tempSocket: Socket? = null

        wifiMutex.withLock {
            try {
                // 2. Get connection details from ViewModel
                val port = viewModel.getPeerPortLaunch() ?: return null
                val ipv6String = viewModel.getIpAddressRemoteLaunch() ?: return null
                val address = Inet6Address.getByName(ipv6String)

                // 3. Create socket bound to the Aware Network
                tempSocket = targetNetwork.socketFactory.createSocket(address, port)

                // 4. Set explicit parameters to avoid compiler inference errors
                tempSocket?.soTimeout = 5000   // 5 second wait for ESP32 handshake
                tempSocket?.tcpNoDelay = true // Send immediately

                // 5. Send Macro and Data
                val output = DataOutputStream(tempSocket!!.getOutputStream())
                output.writeByte(macro)

                // 6. Await for receive the data

                val inputStream = tempSocket!!.getInputStream()
                val response = inputStream.readAllBytes()
                output.flush()


                // Returns
                return response

            } catch (e: Exception) {
                println("Socket Error: ${e.message}")
                Toast.makeText(context, R.string.connectionError, Toast.LENGTH_SHORT).show()

                return null
            } finally {
                // 7. Cleanup: Always close the socket
                try { tempSocket?.close() } catch (e: Exception) { }
            }
        }
    }

    @RequiresPermission(allOf = [Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.NEARBY_WIFI_DEVICES, Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.NEARBY_WIFI_DEVICES])
    @RequiresApi(Build.VERSION_CODES.TIRAMISU)
    // Inside your WifiAware class
    suspend fun startWiFiAwareandSubscribe() {
        coroutineScope {
            // 1. Regular call
            discover()

            // 2. Create the "Signal" object
            val attachedSignal = CompletableDeferred<Boolean>()

            // 3. Pass the signal to your function
            attachToWifi(attachedSignal)

            // 4. WAIT here until the hardware responds (onAttached or onAttachFailed)
            val success = attachedSignal.await()

            if (success) {
                // 5. Now it is safe to call subscribe because wifiAwareSession is NOT null
                subscribe()
            } else {

                Toast.makeText(context, R.string.connectionError, Toast.LENGTH_SHORT).show()
                println("Could not attach to hardware")
            }
        }
    }


    fun closeSession() {
        try {
            // 1. Close the active Socket to stop any pending transmissions
            socket?.close()
            socket = null

            // 2. Clear the Network object to prevent sendData() from running
            currentNetwork = null

            // 3. Explicitly close discovery and main Aware sessions
            subscribeDiscoverySession?.close()
            subscribeDiscoverySession = null

            wifiAwareSession?.close()
            wifiAwareSession = null


            println("Wi-Fi Aware session and UI state fully reset.")
        } catch (e: Exception) {
            Toast.makeText(context, R.string.connectionError, Toast.LENGTH_SHORT).show()
            println("Error during closeSession cleanup: ${e.message}")
        }
    }

suspend fun sendSchedulerToEsp32(viewModel: DeviceSchedulerViewModel) {


     val listDevices = viewModel.devicesList()
     val arrayData: ByteArray= ByteArray(listDevices.size * 5)

    for (i in 0 until  listDevices.size){
        arrayData[i*5] = listDevices[i].device_number!!.toByte()
        arrayData[i*5+1] = listDevices[i].hour_on!!.toByte()
        arrayData[i*5+2] = listDevices[i].minutes_on!!.toByte()
        arrayData[i*5+3] = listDevices[i].hour_off!!.toByte()
        arrayData[i*5+4] = listDevices[i].minutes_off!!.toByte()
    }
        sendData( RECEIVED_SCHEDULER, arrayData)

}
    @RequiresApi(Build.VERSION_CODES.TIRAMISU)
    suspend fun requestVoltages():List<Int>?{
        /* order of voltages are 4 int so
        Device1 byArray[0,1],
        Device2 byArray[2,3],
        Device3 byArray[4,5],
        battery_Voltage byArray[6,7]
        Ac_Voltage byArray[8,9],

         */
        var byteArray: ByteArray = ByteArray(10)
        byteArray= sendDataWithReturnData(RECEIVED_REPORT_REQ)?: return null
        val list= mutableListOf<Int>()
        var temp:Int=0

        for (i in 0 until byteArray.size/2){
            temp = byteArray[2*i].toInt()
            temp = byteArray[2*i+1].toInt().shl(8)
            list.add(temp)
        }
        return list.toList()
    }
    suspend fun sendMatchFilterToESP32(byteArray: ByteArray){

        sendData( RECEIVED_MATCH_FILTER, byteArray)
    }
}



