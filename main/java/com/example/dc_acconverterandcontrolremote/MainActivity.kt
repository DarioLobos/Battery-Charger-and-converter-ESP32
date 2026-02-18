package com.example.dc_acconverterandcontrolremote
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import com.example.dc_acconverterandcontrolremote.ui.theme.DC_ACConverterAndControlRemoteTheme


class MainActivity : ComponentActivity() {



    override fun onCreate(savedInstanceState: Bundle?) {

        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            DC_ACConverterAndControlRemoteTheme {
            }
        }


        setContent {
            DC_ACConverterAndControlRemoteTheme {

                DatabaseApplication()
                MainApp(this)


            }
        }

    }
}



