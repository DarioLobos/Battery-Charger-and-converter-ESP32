package com.example.dc_acconverterandcontrolremote
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.sp
import androidx.compose.ui.Alignment
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.grid.GridCells
import androidx.compose.foundation.lazy.grid.LazyVerticalGrid
import androidx.compose.material3.*
import android.content.Context
import androidx.compose.foundation.background
import androidx.compose.ui.unit.dp
import kotlinx.coroutines.*
import androidx.constraintlayout.compose.ConstraintLayout
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.ui.res.stringResource
import androidx.constraintlayout.compose.ChainStyle
import com.example.dc_acconverterandcontrolremote.R
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.lifecycle.compose.collectAsStateWithLifecycle
import com.example.dc_acconverterandcontrolremote.DevicesDatabase.Companion.DevicesDataBase
import kotlinx.coroutines.flow.toList

// 1. Logic-only Button Component
@Composable
fun DeviceButton(
    deviceNumber: Int,
    isOn: Boolean,
    modifier: Modifier = Modifier,
    model: DeviceSchedulerViewModel,
    aware: WifiAware
) {
    val text = stringResource(if (isOn) R.string.ON else R.string.OFF)

    ElevatedButton(
        onClick = { model.sendActionToWiFI(deviceNumber, isOn, aware) },
        modifier = modifier.wrapContentSize(),
        shape = MaterialTheme.shapes.extraLarge
    ) {
        Text(text, fontSize = 20.sp)
    }
}

// 2. The Layout Component
@Composable
fun DeviceControlCard(
    deviceNumber: Int,
    deviceName: String,
    model: DeviceSchedulerViewModel,
    aware: WifiAware
) {
    ConstraintLayout(
        modifier = Modifier
            .wrapContentSize()
            .background(MaterialTheme.colorScheme.background)
    ) {
        val (buttonOn, buttonOff, titleName) = createRefs()

        val modifierText: Modifier = Modifier
            .constrainAs(titleName) {
                top.linkTo(parent.top, margin = 10.dp)
                bottom.linkTo(buttonOff.top, margin = 10.dp)
                centerHorizontallyTo(parent)
            }

        val modifierOn: Modifier = Modifier
            .constrainAs(buttonOn) {
                top.linkTo(titleName.bottom, margin = 5.dp)
                bottom.linkTo(parent.bottom, margin = 5.dp)
                end.linkTo(buttonOff.start)

            }
        val modifierOff: Modifier = Modifier
            .constrainAs(buttonOff) {
                top.linkTo(titleName.bottom, margin = 5.dp)
                bottom.linkTo(parent.bottom, margin = 5.dp)
                start.linkTo(buttonOn.end)
            }


        Text(
            text = deviceName,
            modifier = modifierText
        )

        DeviceButton(
            deviceNumber, true,
            modifier = modifierOn,
            model, aware
        )

        DeviceButton(
            deviceNumber, false,
            modifier = modifierOff,
            model, aware
        )

        createHorizontalChain(buttonOn, buttonOff, chainStyle = ChainStyle.SpreadInside)
    }
}

@Composable
fun MainScreen(context: Context, model: DeviceSchedulerViewModel, aware: WifiAware) {
    // Collect the StateFlow from your ViewModel
    val isReady by model.isInitialized.collectAsStateWithLifecycle()

    LaunchedEffect(Unit) {
          if(!model.isInitialized.value) {
              model.devicesInit()
          }
    }

    if (!isReady) {
        // Center the loader
        Box(modifier = Modifier.fillMaxSize(), contentAlignment = Alignment.Center) {
            CircularProgressIndicator()
        }
    } else {
        val devices = model.devicesList()

        LazyVerticalGrid(
            columns = GridCells.Fixed(2),
            modifier = Modifier.fillMaxSize(),
            horizontalArrangement = Arrangement.SpaceEvenly,
            verticalArrangement = Arrangement.spacedBy(20.dp),
            contentPadding = PaddingValues(
                bottom = 100.dp // High enough gap for your Floating Button
            )
        ){
            items(devices.size) { index ->
                // Make sure this name matches your defined function below
                DeviceControlCard(
                    deviceNumber = index,
                    deviceName = devices[index].device_name!!,
                    model = model,
                    aware = aware
                )
            }
        }
    }
}