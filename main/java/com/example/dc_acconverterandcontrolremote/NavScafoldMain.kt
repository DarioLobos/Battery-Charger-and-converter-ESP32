package com.example.dc_acconverterandcontrolremote
import com.example.dc_acconverterandcontrolremote.R
import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.os.Build
import android.widget.Toast
import androidx.annotation.RequiresApi
import androidx.annotation.RequiresPermission
import androidx.compose.material3.Icon
import androidx.compose.material3.Text
import androidx.compose.ui.res.stringResource
import androidx.compose.material.icons.filled.Menu
import androidx.compose.runtime.rememberCoroutineScope
import androidx.annotation.StringRes
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.AddCircle
import androidx.compose.material.icons.filled.Build
import androidx.compose.material.icons.filled.CheckCircle
import androidx.compose.material.icons.filled.Refresh
import androidx.compose.material.icons.filled.Settings
import androidx.compose.material3.FloatingActionButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.adaptive.navigationsuite.NavigationSuiteScaffold
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import androidx.core.app.ActivityCompat
import kotlinx.coroutines.launch


enum class MenuList(
    @StringRes val label: Int,
    val icon: ImageVector,
    @StringRes val contentDescription: Int
    ) {
        HOME(R.string.first_fragment_label, Icons.Default.Menu, R.string.Button_list),
        VOLTAGES(R.string.voltage_fragment_label, Icons.Default.CheckCircle, R.string.Voltage_list),
        CHARGERSCHEDULER(R.string.char_scheduler_fragment_label, Icons.Default.Build, R.string.char_scheduler_list),
        DEVICESSCHEDULER(R.string.dev_scheduler_fragment_label, Icons.Default.AddCircle, R.string.dev_scheduler_list),
        SETTINGS(R.string.action_settings, Icons.Default.Settings, R.string.action_settings)
    }

@RequiresApi(Build.VERSION_CODES.TIRAMISU)
@RequiresPermission(Manifest.permission.ACCESS_WIFI_STATE)
@Composable
fun MainApp(context: Context, viewModel: DeviceSchedulerViewModel, aware: WifiAware){
    var currentDestination by rememberSaveable { mutableStateOf(MenuList.HOME)}
    val scope = rememberCoroutineScope()

    NavigationSuiteScaffold(
        navigationSuiteItems = {
            MenuList.entries.forEach {
                item(
                    icon = {
                        Icon(it.icon, contentDescription = stringResource(it.contentDescription))
                    },
                    label = { Text(stringResource(it.label)) },
                    selected = it == currentDestination,
                    onClick = { currentDestination = it }
                )
            }
        }
    ){
        // 1. Wrap in a Box so the FAB can float in the bottom corner
        Box(modifier = Modifier.fillMaxSize()) {

            val localContext = LocalContext.current

            // 2. The main screen content
            when (currentDestination) {
                // Pass 'aware' here to match our new MainScreen definition
                MenuList.HOME -> MainScreen(localContext, viewModel, aware)
                MenuList.VOLTAGES -> Voltage_Screen()
                MenuList.CHARGERSCHEDULER -> ChargerScheduler_Screen()
                MenuList.DEVICESSCHEDULER -> DeviceScheduler_Screen( context, viewModel, aware)
                MenuList.SETTINGS -> Settings_Screen(viewModel, localContext, aware)
            }

            // 3. The FAB with correct modifier syntax
            FloatingActionButton(
                containerColor = MaterialTheme.colorScheme.primary,
                modifier = Modifier
                    .align(Alignment.BottomEnd) // Works because it's in a Box
                    .padding(16.dp),
                onClick = {
                    scope.launch {
                        if (ActivityCompat.checkSelfPermission(
                                context,
                                Manifest.permission.ACCESS_FINE_LOCATION
                            ) != PackageManager.PERMISSION_GRANTED || ActivityCompat.checkSelfPermission(
                                context,
                                Manifest.permission.NEARBY_WIFI_DEVICES
                            ) != PackageManager.PERMISSION_GRANTED
                        ) {
                           println("aware permission error")
                            Toast.makeText(context, "WIFI permission failed. Try again.", Toast.LENGTH_SHORT).show()
                        }
                        aware.startWiFiAwareandSubscribe()
                    }
                }
            ) {
                // Fixed: Removed the { }
                Icon(
                    Icons.Filled.Refresh,
                    contentDescription = stringResource(R.string.checkConnection)
                )
            }
        }
    }
}