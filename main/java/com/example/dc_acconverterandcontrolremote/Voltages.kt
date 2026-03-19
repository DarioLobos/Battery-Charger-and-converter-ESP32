package com.example.dc_acconverterandcontrolremote
import android.Manifest
import com.example.dc_acconverterandcontrolremote.R
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.tooling.preview.Preview
import com.example.dc_acconverterandcontrolremote.ui.theme.DC_ACConverterAndControlRemoteTheme
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.foundation.border
import androidx.compose.foundation.layout.*
import androidx.compose.ui.*
import androidx.compose.foundation.lazy.grid.GridCells
import androidx.compose.foundation.lazy.grid.LazyHorizontalGrid
import androidx.compose.foundation.lazy.grid.LazyVerticalGrid
import androidx.compose.foundation.lazy.grid.items // For lists of items
import androidx.compose.material3.*
import android.content.Context
import android.content.pm.PackageManager
import android.os.Build
import android.widget.Toast
import androidx.compose.foundation.background
import androidx.compose.foundation.gestures.ScrollableState
import androidx.compose.ui.unit.dp
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.flow.map
import java.nio.file.WatchEvent
import androidx.constraintlayout.compose.*
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.MoreVert
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.graphics.Outline
import androidx.compose.ui.res.stringResource
import androidx.activity.enableEdgeToEdge
import androidx.annotation.RequiresApi
import androidx.collection.emptyLongSet
import androidx.datastore.preferences.core.edit
import androidx.datastore.preferences.core.intPreferencesKey
import androidx.datastore.preferences.preferencesDataStore
import androidx.room.Room
import androidx.compose.runtime.*
import androidx.compose.ui.focus.onFocusChanged
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.AnnotatedString
import androidx.compose.ui.text.input.OffsetMapping
import androidx.compose.ui.text.input.TransformedText
import androidx.compose.ui.text.input.VisualTransformation
import androidx.core.app.ActivityCompat
import kotlinx.coroutines.TimeoutCancellationException
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import kotlinx.coroutines.withTimeout


@RequiresApi(Build.VERSION_CODES.TIRAMISU)
@Composable
fun Voltage_Screen(context: Context, viewModel: DeviceSchedulerViewModel, aware: WifiAware){

    val scope = rememberCoroutineScope()

    var voltageDevice1 by remember { mutableStateOf(  "0.00")}
    var voltageDevice2 by remember { mutableStateOf(  "0.00")}
    var voltageDevice3 by remember { mutableStateOf(  "0.00")}
    var voltageAC by remember { mutableStateOf(  "0.00")}
    var voltageBattery by remember { mutableStateOf(  "0.00")}

    Box(propagateMinConstraints = false) {

        }
        Column(
            modifier = Modifier
                .wrapContentSize(),
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.SpaceEvenly
        ) {
                ElevatedButton(
                    onClick = {
                        scope.launch {
                            val list = aware.requestVoltages()
                            if (list !=null){
                                var temp1:Int=(list[0]%100)?:0
                                var temp2:Int=((list[0]-list[0]%100)/100)?:0
                                voltageDevice1 = "$temp2'.'$temp1"

                                temp1=(list[1]%100)?:0
                                temp2=((list[1]-list[0]%100)/100)?:0
                                voltageDevice2 = "$temp2'.'$temp1"

                                temp1=(list[2]%100)?:0
                                temp2=((list[2]-list[0]%100)/100)?:0
                                voltageDevice3= "$temp2'.'$temp1"

                                temp1=(list[3]%100)?:0
                                temp2=((list[3]-list[0]%100)/100)?:0
                                voltageBattery = "$temp2'.'$temp1"

                                temp1=(list[4]%100)?:0
                                temp2=((list[4]-list[0]%100)/100)?:0
                                voltageAC= "$temp2'.'$temp1"

                            }else{
                                Toast.makeText(context, R.string.connectionError, Toast.LENGTH_SHORT).show()

                            }

                        }
                    },
                    modifier = Modifier.wrapContentSize(),
                    shape = MaterialTheme.shapes.extraLarge
                ) {
                    Text( stringResource(R.string.requestVoltages) , fontSize = 20.sp)
                }
                Text(
                    text = stringResource(R.string.sensed),
                    style = MaterialTheme.typography.titleLarge)

                Text(
                    text = "${stringResource(R.string.device1)}  $voltageDevice1",
                    style = MaterialTheme.typography.bodyLarge)

                Text(
                    text = "${stringResource(R.string.device2)}  $voltageDevice1",
                    style = MaterialTheme.typography.bodyLarge)

                Text(
                    text = "${stringResource(R.string.device3)}  $voltageDevice1",
                    style = MaterialTheme.typography.bodyLarge)

                Text(
                    text = "${stringResource(R.string.AC)}  $voltageDevice1",
                    style = MaterialTheme.typography.bodyLarge)

                Text(
                    text = "${stringResource(R.string.battery)}  $voltageDevice1",
                    style = MaterialTheme.typography.bodyLarge)

    }

}
