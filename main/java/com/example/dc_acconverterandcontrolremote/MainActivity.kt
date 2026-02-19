package com.example.dc_acconverterandcontrolremote
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.activity.viewModels
import androidx.compose.ui.platform.LocalContext
import com.example.dc_acconverterandcontrolremote.DevicesDatabase.Companion.DevicesDataBase
import com.example.dc_acconverterandcontrolremote.ui.theme.DC_ACConverterAndControlRemoteTheme


class MainActivity : ComponentActivity() {

    private val model: DeviceSchedulerViewModel by viewModels()
    override fun onCreate(savedInstanceState: Bundle?) {

        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            DC_ACConverterAndControlRemoteTheme {
            }
        }


        setContent {
            DC_ACConverterAndControlRemoteTheme {
                model.devicesDao= DevicesDataBase(this).daoDevices()
                DatabaseApplication()
                MainApp(this,model )


            }
        }

    }
}



