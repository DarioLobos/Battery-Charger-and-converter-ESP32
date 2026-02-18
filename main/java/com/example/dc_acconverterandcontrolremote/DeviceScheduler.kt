package com.example.dc_acconverterandcontrolremote
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.foundation.layout.*
import androidx.compose.ui.*
import androidx.compose.foundation.lazy.grid.GridCells
import androidx.compose.foundation.lazy.grid.LazyVerticalGrid
import androidx.compose.material3.*
import android.content.Context
import android.widget.Toast
import androidx.compose.foundation.background
import androidx.compose.ui.unit.dp
import androidx.constraintlayout.compose.*
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.runtime.remember
import androidx.compose.ui.res.stringResource
import androidx.compose.runtime.*
import androidx.compose.material3.TimePicker
import androidx.compose.material3.TimePickerState
import androidx.compose.material3.rememberTimePickerState
import androidx.compose.material3.MultiChoiceSegmentedButtonRow
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.tooling.preview.PreviewParameter
import java.util.*
import androidx.compose.ui.platform.LocalContext
import androidx.compose.foundation.clickable
import androidx.compose.ui.modifier.modifierLocalConsumer
import androidx.lifecycle.ViewModel

@Composable
fun dataFromViewModel(model: DeviceSchedulerViewModel){

    val context: Context = LocalContext.current
    var device_number:Int by model.device_number.observeASState(-1)

    DeviceScheduler_Screen(context, model.days_Selected,
        model.setDeviceNumber(device_number),
        model.on_or_off, model.setOnorOff(),
        ,model.hourSet, model.minuteSet,model.hourForPicker, model.minuteForPicker,
        model.imesToshow, model.setselectedTime(),model.deviceName(),
        model.selectedDays())

}

    @OptIn(ExperimentalMaterial3Api::class)
    @Composable
    fun EditTextONOFF(context: Context,device_number: Int, setdevice_number:(device_number: Int)->Unit,
                      on_or_off:String,seton_or_off:(on_or_off:String)->Unit, modifier: Modifier,
                      hourSet: Int, minuteSet: Int,hourForPicker: Int, minuteForPicker: Int,
                      TimesToshow:(device_number: Int, on_or_off: String, string_onoff: String?)-> String
                      setselectedTime:(hourToSeT: Int, minuteToSet: Int, device_number: Int,
                                       on_or_off: String, string_onoff: String)-> String) {

        val string_onoff: String = stringResource(R.string.ON)
        var timeToShow: String = TimesToshow(device_number,on_or_off,string_onoff)?:stringResource(R.string.click)
        var selectedTime: TimePickerState? by remember { mutableStateOf(null) }
        var showTimePicker by remember { mutableStateOf(true) }
        val calendar = Calendar.getInstance()
        val hourForPicker: Int = hourSet ?: calendar[Calendar.HOUR_OF_DAY]
        val minuteForPicker: Int = minuteSet ?: calendar[Calendar.MINUTE]
        val device: List<Devices> = devicesDao?.getItem(device_number) as List<Devices>


        val timePickerState = rememberTimePickerState(
            initialHour = hourForPicker,
            initialMinute = minuteForPicker,
            is24Hour = true
        )

        Box(propagateMinConstraints = false) {
            Column(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(32.dp),
                horizontalAlignment = Alignment.CenterHorizontally,
                verticalArrangement = Arrangement.spacedBy(24.dp),
            ) {
                showTimePicker = true

                // Source - https://stackoverflow.com/a/79889530
// Posted by tyg, modified by community. See post 'Timeline' for change history
// Retrieved 2026-02-16, License - CC BY-SA 4.0

                TextField(
                    // colors = MaterialTheme.colorScheme.onPrimary,
                    readOnly = false,
                    enabled = false,
                    value = timeToShow,
                    onValueChange = { /* ... */ },
                    label = { Text(on_or_off) },
                    singleLine = true,
                    placeholder = { Text(stringResource(R.string.click)) },
                    modifier = Modifier.clickable {
                        showTimePicker = true
                    },
                )


                if (showTimePicker) {

                    TimePicker(
                        state = timePickerState,
                        Modifier.fillMaxSize()
                    )
                    Button(

                        onClick = {
                            showTimePicker = false
                            Toast.makeText(context, R.string.setTimeCancel, Toast.LENGTH_SHORT).show()
                        })
                    {
                        Text(stringResource(R.string.cancel))
                    }

                    Button(
                        onClick = {
                            if (timePickerState!=null){
                            setselectedTime(timePickerState.hour, timePickerState.minute,
                                device_number, on_or_off, string_onoff)}
                            showTimePicker = false
                            Toast.makeText(context, R.string.setTimeRecorded, Toast.LENGTH_SHORT).show()
                        }) {
                        Text(stringResource(R.string.confirm))


                    }
                }
            }
        }
    }


    @Composable
    fun ConstrainWithEditTextOnOff(context: Context,device_number: Int, setdevice_number:(device_number: Int)->Unit,
                                   on_or_off:String,seton_or_off:(on_or_off:String)->Unit,
                                   hourSet: Int, minuteSet: Int,hourForPicker: Int, minuteForPicker: Int,
                                   TimesToshow:(device_number: Int, on_or_off: String, string_onoff: String?)-> String
                                   setselectedTime:(hourToSeT: Int, minuteToSet: Int, device_number: Int, on_or_off: String, string_onoff: String)-> String,
                                   deviceName:(device_number:Int)-> String, selectedDays:( option:Int, selectectOptions : Boolean)->Unit ) {


        ConstraintLayout(
            Modifier
                .wrapContentSize()
                .background(color = MaterialTheme.colorScheme.background)
        ) {

            val on: String = stringResource(R.string.ON)
            val off: String = stringResource(R.string.OFF)
            val (editOn, editOff, titleName, timeOn, timeOff, daysOfWeek) = createRefs()

            val modifierText: Modifier = Modifier
                .constrainAs(titleName) {
                    top.linkTo(parent.top, margin = 10.dp)
                    bottom.linkTo(editOff.top, margin = 10.dp)
                    centerHorizontallyTo(parent)
                }

            val modifierTextON: Modifier = Modifier
                .constrainAs(timeOn) {
                    top.linkTo(titleName.bottom, margin = 5.dp)
                    bottom.linkTo(daysOfWeek.top, margin = 5.dp)
                    end.linkTo(editOn.start)
                }

            val modifierEditOn: Modifier = Modifier
                .constrainAs(editOn) {
                    top.linkTo(titleName.bottom, margin = 5.dp)
                    bottom.linkTo(daysOfWeek.top, margin = 5.dp)
                    start.linkTo(timeOn.end)
                    end.linkTo(editOff.start)
                }

            val modifierTextOFF: Modifier = Modifier
                .constrainAs(timeOff) {
                    top.linkTo(titleName.bottom, margin = 5.dp)
                    bottom.linkTo(daysOfWeek.top, margin = 5.dp)
                    start.linkTo(editOn.end)
                    end.linkTo(editOff.start)
                }

            val modifierEditOff: Modifier = Modifier
                .constrainAs(editOff) {
                    top.linkTo(titleName.bottom, margin = 5.dp)
                    bottom.linkTo(daysOfWeek.top, margin = 5.dp)
                    start.linkTo(timeOff.end)
                }


            val modifierRButtonDaysOfWeek = Modifier
                .wrapContentSize()
                .constrainAs(daysOfWeek) {
                    top.linkTo(editOn.bottom, margin = 5.dp)
                    bottom.linkTo(parent.bottom, margin = 5.dp)
                    centerHorizontallyTo(parent)
                }


            Text(
                text = deviceName(device_number),
                color = MaterialTheme.colorScheme.onPrimary,
                style = MaterialTheme.typography.bodyLarge,
                fontWeight = FontWeight.Bold,
                modifier = modifierText
            )
            Text(
                text = stringResource(R.string.timeOn),
                color = MaterialTheme.colorScheme.onPrimary,
                style = MaterialTheme.typography.bodyMedium,
                fontWeight = FontWeight.SemiBold,
                modifier = modifierTextON
            )
            Text(
                text = stringResource(R.string.timeOFF),
                color = MaterialTheme.colorScheme.onPrimary,
                style = MaterialTheme.typography.bodyLarge,
                fontWeight = FontWeight.Bold,
                modifier = modifierTextOFF
            )
            EditTextONOFF(context, device_number,setdevice_number, on, seton_or_off,
                modifierEditOn, hourSet, minuteSet,hourForPicker, minuteForPicker,
                TimesToshow,setselectedTime)
            EditTextONOFF(context, device_number,setdevice_number, off,
                seton_or_off, modifierEditOff, hourSet, minuteSet,hourForPicker,
                minuteForPicker, TimesToshow,setselectedTime)
            createHorizontalChain(
                editOn, editOff,
                chainStyle = ChainStyle.SpreadInside
            )

            var days_Selected: Int = 0


            val options = listOf(
                stringResource(R.string.Sun), stringResource(R.string.Mon),
                stringResource(R.string.Tue), stringResource(R.string.Wed),
                stringResource(R.string.Thu), stringResource(R.string.Fri),
                stringResource(R.string.Sat)
            )

            val selectedOptions= remember {mutableStateListOf(false, false, false, false, false, false, false)}

                MultiChoiceSegmentedButtonRow(modifier = modifierRButtonDaysOfWeek) {
                options.forEachIndexed { index, label ->
                    SegmentedButton(
                        shape = SegmentedButtonDefaults.itemShape(
                            index = index,
                            count = options.size
                        ),
                        checked = selectedOptions[index],
                        onCheckedChange = {
                            selectedOptions[index] = !selectedOptions[index]
                            setdevice_number(device_number)
                            seton_or_off(on_or_off)
                            selectedDays( index, selectedOptions[index])
                        },
                        label =
                            {  }

                    )
                }
            }

        }


    }


    @Composable
    fun DeviceScheduler_Screen(context: Context,device_number: Int, setdevice_number:(device_number: Int)->Unit,
                               on_or_off:String,seton_or_off:(on_or_off:String)->Unit,
                               hourSet: Int, minuteSet: Int,hourForPicker: Int, minuteForPicker: Int,
                               TimesToshow:(device_number: Int, on_or_off: String, string_onoff: String?)-> String
                               setselectedTime:(hourToSeT: Int, minuteToSet: Int, device_number: Int, on_or_off: String, string_onoff: String)-> String,
                               deviceName:(device_number:Int)-> String, selectedDays:( option:Int, selectectOptions : Boolean)->Unit ) {


        val devicesListSize: Int = deviceList()?.size ?: 0

        LazyVerticalGrid(
            columns = GridCells.Fixed(2), Modifier
                .wrapContentSize()
                .fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceEvenly,
            verticalArrangement = Arrangement.spacedBy(20.dp)
        ) {
            items(devicesListSize) {
                setdevice_number(it)
                seton_or_off(on_or_off)
                ConstrainWithEditTextOnOff(context, it, setdevice_number,on_or_off,
                    seton_or_off,hourSet,minuteSet,hourForPicker,minuteForPicker,
                    TimesToshow,setselectedTime, deviceName,selectedDays)
            }
        }
    }


