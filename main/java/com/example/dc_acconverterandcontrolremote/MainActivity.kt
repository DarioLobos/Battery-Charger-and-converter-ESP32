package com.example.dc_acconverterandcontrolremote
import android.Manifest
import android.os.Build
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.activity.viewModels
import androidx.annotation.RequiresApi
import androidx.annotation.RequiresPermission
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.ui.platform.LocalContext
//import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewmodel.compose.viewModel
import com.example.dc_acconverterandcontrolremote.DevicesDatabase.Companion.DevicesDataBase
import com.example.dc_acconverterandcontrolremote.ui.theme.DC_ACConverterAndControlRemoteTheme
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch


class MainActivity : ComponentActivity() {

    //private val model: DeviceSchedulerViewModel by viewModels()
    val viewModel = DeviceSchedulerViewModel()
    val aware = WifiAware(applicationContext, viewModel)

    @RequiresPermission(Manifest.permission.ACCESS_WIFI_STATE)
    @RequiresApi(Build.VERSION_CODES.TIRAMISU)
    override fun onCreate(savedInstanceState: Bundle?) {

        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {

            DC_ACConverterAndControlRemoteTheme {
                DatabaseApplication()
                val context = LocalContext.current
                val modelComposeM3 = viewModel<DeviceSchedulerViewModel>()
                modelComposeM3.devicesDao = DevicesDataBase(applicationContext).daoDevices()
                MainApp(context, modelComposeM3, aware)
            }
        }


    }

    @RequiresPermission(Manifest.permission.ACCESS_WIFI_STATE)
    @RequiresApi(Build.VERSION_CODES.TIRAMISU)
    override fun onResume() {
        super.onResume()

        // these are pending to define
        var macFilter: List<ByteArray> =
            listOf(viewModel.setMacStringToAddress(viewModel.MAC_ADDRESS_REMOTE.toString()))
        // these are pending to define pending define IPV6forphone
        lateinit var serviceSpecificInfo: ByteArray

        GlobalScope.launch {
            aware.startWiFiAwareandSubscribe()
        }
    }

    override fun onPause() {
        super.onPause()
        GlobalScope.launch {
                aware.closeSession()
        }
    }
}