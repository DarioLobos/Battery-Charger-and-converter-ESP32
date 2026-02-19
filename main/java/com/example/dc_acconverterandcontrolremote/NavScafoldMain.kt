package com.example.dc_acconverterandcontrolremote
import android.content.Context
import androidx.compose.material3.Icon
import androidx.compose.material3.Text
import androidx.compose.ui.res.stringResource
import androidx.compose.material.icons.filled.Menu
import androidx.compose.runtime.rememberCoroutineScope
import androidx.annotation.StringRes
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.AddCircle
import androidx.compose.material.icons.filled.Build
import androidx.compose.material.icons.filled.CheckCircle
import androidx.compose.material.icons.filled.Settings
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.adaptive.navigationsuite.NavigationSuiteScaffold
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.platform.LocalContext

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


@Composable
fun MainApp(context: Context, viewModel: DeviceSchedulerViewModel){
    var currentDestination by rememberSaveable { mutableStateOf(MenuList.HOME)}
    val scope = rememberCoroutineScope()

    NavigationSuiteScaffold(
        containerColor = MaterialTheme.colorScheme.primary,
        contentColor = MaterialTheme.colorScheme.onPrimary,
        navigationSuiteItems = {
            MenuList.entries.forEach {
                item(
                    icon = {
                        Icon(
                            it.icon,
                            contentDescription = stringResource(it.contentDescription
                            ))
                    },
                    label = { Text(stringResource(it.label)) },
                    selected = it == currentDestination,
                    onClick = { currentDestination = it }
                )
            }
        }
    ){
        val context = LocalContext.current
        when (currentDestination) {
            MenuList.HOME -> MainScreen (context)
            MenuList.VOLTAGES -> Voltage_Screen()
            MenuList.CHARGERSCHEDULER -> ChargerScheduler_Screen()
            MenuList.DEVICESSCHEDULER ->  DataFromViewModel(viewModel)
            MenuList.SETTINGS -> Settings_Screen()

        }
    }
}