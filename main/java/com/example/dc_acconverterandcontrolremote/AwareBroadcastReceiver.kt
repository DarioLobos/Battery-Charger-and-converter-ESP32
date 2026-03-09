package com.example.dc_acconverterandcontrolremote

import android.Manifest
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.net.wifi.aware.AttachCallback
import android.net.wifi.aware.IdentityChangedListener
import android.net.wifi.aware.WifiAwareManager
import android.net.wifi.aware.WifiAwareSession
import android.os.Build
import androidx.annotation.RequiresApi
import androidx.annotation.RequiresPermission
import androidx.core.app.ActivityCompat

class AwareBroadcastReceiver(val context: Context, val viewModel: DeviceSchedulerViewModel, val wifiAwareManager: WifiAwareManager) : BroadcastReceiver() {



    @RequiresPermission(allOf = [Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.NEARBY_WIFI_DEVICES, Manifest.permission.ACCESS_WIFI_STATE])
    override fun onReceive(context: Context, intent: Intent) {

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            wifiAwareManager.characteristics


        if (wifiAwareManager.isAvailable) {
            println("nanAvailable")
            if (ActivityCompat.checkSelfPermission(
                    context,
                    Manifest.permission.ACCESS_FINE_LOCATION
                ) != PackageManager.PERMISSION_GRANTED || ActivityCompat.checkSelfPermission(
                    context,
                    Manifest.permission.NEARBY_WIFI_DEVICES
                ) != PackageManager.PERMISSION_GRANTED
            ) {
                // TODO: Consider calling
                //    ActivityCompat#requestPermissions
                // here to request the missing permissions, and then overriding
                //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                //                                          int[] grantResults)
                // to handle the case where the user grants the permission. See the documentation
                // for ActivityCompat#requestPermissions for more details.
                return
            }
            wifiAwareManager.attach(object : AttachCallback() {

                override fun onAttached(session: WifiAwareSession) {
                    // Session attached, proceed to publish or subscribe
                }

                override fun onAttachFailed() {


                }

            }, object : IdentityChangedListener(){

                override fun onIdentityChanged(mac: ByteArray?) {
                    super.onIdentityChanged(mac)
                    viewModel.setMacAddress(mac)
                }

            },null)
        }
    }
}

}