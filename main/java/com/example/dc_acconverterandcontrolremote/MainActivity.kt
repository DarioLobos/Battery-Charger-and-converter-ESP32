package com.example.dc_acconverterandcontrolremote
import android.Manifest
import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.activity.viewModels
import androidx.annotation.RequiresApi
import androidx.annotation.RequiresPermission
import androidx.compose.ui.platform.LocalContext
import androidx.core.app.ActivityCompat
import androidx.lifecycle.lifecycleScope
import com.example.dc_acconverterandcontrolremote.ui.theme.DC_ACConverterAndControlRemoteTheme
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.collectLatest
import kotlinx.coroutines.launch


class MainActivity : ComponentActivity() {

    // 1. Correct ViewModel Initialization
    // This uses the Factory we built to safely get the Repo from the REAL Application instance
    private val viewModel: DeviceSchedulerViewModel by viewModels {
        DeviceSchedulerViewModel.Factory
    }

    // 2. Lateinit for WifiAware
    // We must initialize this in onCreate after the ViewModel is ready
    private lateinit var aware: WifiAware

    @RequiresPermission(Manifest.permission.ACCESS_WIFI_STATE)
    @RequiresApi(Build.VERSION_CODES.TIRAMISU)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // 3. Initialize WifiAware using the system-provided applicationContext
        aware = WifiAware(applicationContext, viewModel)

        enableEdgeToEdge()
        setContent {
            DC_ACConverterAndControlRemoteTheme {
                // Pass the lifecycle-managed viewModel and aware instance
                MainApp(LocalContext.current, viewModel, aware)
            }
        }
    }

    @RequiresPermission(Manifest.permission.ACCESS_WIFI_STATE)
    @RequiresApi(Build.VERSION_CODES.TIRAMISU)
    override fun onResume() {
        super.onResume()
        lifecycleScope.launch {
            // 2. Observe your discoverySettings (Match Filter + DB Status)
            viewModel.discoverySettings.collectLatest { settings ->

                // 3. WAIT: Only start if storage/DB are verified 'Ready'
                if (settings.isReady) {
                    // If a session already exists (e.g., from a settings change), close it first
                    if (aware.wifiAwareSession != null) {
                        aware.closeSession()
                        delay(250)
                    }

                    // 4. Start only with verified data
                    if (ActivityCompat.checkSelfPermission(
                            applicationContext,
                            Manifest.permission.ACCESS_FINE_LOCATION
                        ) != PackageManager.PERMISSION_GRANTED || ActivityCompat.checkSelfPermission(
                            applicationContext,
                            Manifest.permission.NEARBY_WIFI_DEVICES
                        ) != PackageManager.PERMISSION_GRANTED
                    ) {
                        Toast.makeText(applicationContext, "WIFI permission failed. Try again.", Toast.LENGTH_SHORT).show()
                    }
                    aware.startWiFiAwareandSubscribe()
                }
            }
        }
    }

    override fun onPause() {
        super.onPause()
        // The lifecycleScope will cancel the 'collectLatest' above automatically,
        // but we still manually close the hardware session to be safe.
        lifecycleScope.launch {
            aware.closeSession()
        }
    }
}