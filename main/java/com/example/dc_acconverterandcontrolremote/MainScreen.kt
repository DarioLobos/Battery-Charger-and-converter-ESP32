package com.example.dc_acconverterandcontrolremote
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.sp
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.grid.GridCells
import androidx.compose.foundation.lazy.grid.LazyVerticalGrid
import androidx.compose.material3.*
import android.content.Context
import androidx.compose.foundation.background
import androidx.compose.ui.unit.dp
import kotlinx.coroutines.*
import androidx.constraintlayout.compose.*
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.ui.res.stringResource
import com.example.dc_acconverterandcontrolremote.DevicesDatabase.Companion.DevicesDataBase


@Composable
fun ButtonstoONOFF(device_number : Int, on_or_off : String, modifier: Modifier, model: DeviceSchedulerViewModel) {

    ElevatedButton(
        onClick = {
        model.sendActionToWiFI(device_number, on_or_off)
        },
        shape = MaterialTheme.shapes.extraLarge,
        colors = ButtonDefaults.buttonColors(
            containerColor = MaterialTheme.colorScheme.primary,
            contentColor = MaterialTheme.colorScheme.onPrimary
        ),
        content = { Text(on_or_off, color = Color.Black, fontSize = 20.sp) },
        modifier = modifier.wrapContentSize()
    )

    }

@Composable
fun ConstraionWithButtonsOnOff(device_number:Int, device_name:String, model: DeviceSchedulerViewModel) {

    ConstraintLayout(Modifier
        .wrapContentSize()
        .background(color = MaterialTheme.colorScheme.background)) {

        val on: String = stringResource(R.string.ON)
        val off: String = stringResource(R.string.OFF)
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

        Text(text = device_name,
            color= MaterialTheme.colorScheme.onPrimary,
            style = MaterialTheme.typography.bodyLarge,
            fontWeight = FontWeight.Bold,
            modifier = modifierText)
        ButtonstoONOFF(device_number, on, modifierOn, model)
        ButtonstoONOFF(device_number, off, modifierOff, model)
        createHorizontalChain(
            buttonOn, buttonOff,
            chainStyle = ChainStyle.SpreadInside
        )
    }

}
@Composable
fun LazyGridForButtonsMain(model: DeviceSchedulerViewModel){

        val devicesListSize: Int = model.deviceList()?.size ?: 0

        LazyVerticalGrid(columns = GridCells.Fixed(2),
            horizontalArrangement = Arrangement.SpaceEvenly,
            verticalArrangement = Arrangement.spacedBy(20.dp)
        ) {
            items(devicesListSize){
            ConstraionWithButtonsOnOff(it, model.deviceName(it)?:"" ,model)
            }
        }
    }

@Composable
fun MainScreen (context: Context, model: DeviceSchedulerViewModel){

    val coroutineScope = rememberCoroutineScope()
    LaunchedEffect(key1=Unit) {
        runBlocking {

            launch {


                if (model.devicesDao.getAll().count() == 0) {
                    model.devicesInit(context)
                    model.deviceListInit()
                }
            }
        }
    }

    LazyGridForButtonsMain(model)
}