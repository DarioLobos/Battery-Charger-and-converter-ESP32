package com.example.dc_acconverterandcontrolremote
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.foundation.layout.*
import androidx.compose.ui.*
import androidx.compose.ui.res.stringResource
import com.example.dc_acconverterandcontrolremote.R
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
//import androidx.compose.material3.TimePickerState
import androidx.compose.material3.rememberTimePickerState
import androidx.compose.material3.MultiChoiceSegmentedButtonRow
import androidx.compose.ui.platform.LocalContext
import androidx.compose.foundation.clickable
import androidx.lifecycle.MutableLiveData
//import androidx.lifecycle.Observer
//import androidx.lifecycle.ViewModel
import kotlin.Int

@Composable
fun DataFromViewModel(model: DeviceSchedulerViewModel) {

    val context: Context = LocalContext.current

    //val device_numberObserver = Observer<Int> {}
    //val on_or_offObserver = Observer<String> {}
    //val deviceObserver = Observer<String> {}
    //val hourSetObserver = Observer<Int> {}
    //val minuteSet = Observer<Int> {}


    val deviceScheduler= DeviceScheduler_Screen(
        model,context, model.hourForPicker, model.minuteForPicker, model::TimesToshow,
        model::setselectedTime, model::selectedDays, model::deviceName)

    deviceScheduler.deviceScheduler_Screen()

}


class EditTextONOFF (val context: Context,val device_number: Int,
val on_or_off:String,val modifier: Modifier,val
hourForPicker: Int,val minuteForPicker: Int,
val TimesToshow:(device_number: Int, on_or_off: String, string_onoff: String)-> String?,
val setselectedTime:(hourToSeT: Int,minuteToSet: Int,device_number: Int,on_or_off: String,string_onoff: String)-> String) {

    @OptIn(ExperimentalMaterial3Api::class)
    @Composable
    fun editTextONOFF() {

        val device_number_keeper: Int = device_number
        val on_or_off_keeper: String = on_or_off
        val string_onoff: String = stringResource(R.string.ON)
        var timeToShow by remember { mutableStateOf("") }
        timeToShow = TimesToshow(device_number_keeper, on_or_off_keeper, string_onoff)
            ?: stringResource(R.string.click)
//        var selectedTime: TimePickerState? by remember { mutableStateOf(null) }
        var showTimePicker by remember { mutableStateOf(true) }

        val timePickerState = rememberTimePickerState(
            initialHour = hourForPicker,
            initialMinute = minuteForPicker,
            is24Hour = true
        )

        Box(propagateMinConstraints = false) {
            Column(
                modifier = Modifier
                    .wrapContentSize(),
                horizontalAlignment = Alignment.CenterHorizontally
            ) {
                showTimePicker = true

                TextField(
                    readOnly = false,
                    enabled = false,
                    value = timeToShow,
                    onValueChange = { /* ... */ },
                    label = { Text(on_or_off) },
                    singleLine = true,
                    placeholder = { Text(stringResource(R.string.click)) },
                    modifier = modifier.clickable {
                        showTimePicker = true
                    }
                )


                if (showTimePicker) {

                    TimePicker(
                        state = timePickerState,

                        Modifier.fillMaxSize()
                    )
                    Button(

                        onClick = {
                            showTimePicker = false
                            Toast.makeText(context, R.string.setTimeCancel, Toast.LENGTH_SHORT)
                                .show()
                        })
                    {
                        Text(stringResource(R.string.cancel))
                    }

                    Button(
                        onClick = {

                            setselectedTime(
                                timePickerState.hour, timePickerState.minute,
                                device_number_keeper, on_or_off_keeper, string_onoff
                            )
                            showTimePicker = false
                            Toast.makeText(context, R.string.setTimeRecorded, Toast.LENGTH_SHORT)
                                .show()
                        }) {
                        Text(stringResource(R.string.confirm))


                    }
                }
            }
        }
    }

}

class ConstrainWithEditTextOnOff(val context: Context, val device_number: Int,
                                 val hourForPicker: Int, val  minuteForPicker: Int,
                                 val TimesToshow:(device_number: Int, on_or_off: String, string_onoff: String)-> String?,
                                 val setselectedTime:(hourToSeT: Int,minuteToSet: Int,device_number: Int,on_or_off: String,string_onoff: String)-> String,
                                 val selectedDays:(daysselected: Int, option: Int, selectedOptions: Boolean)->Unit,
                                 val deviceName:(devicenbr:Int )->String) {

    @Composable
    fun constrainWithEditTextOnOff() {

        val device_number_keeper: Int = device_number

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
                text = deviceName(device_number_keeper),
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
            val editTextOn: EditTextONOFF = EditTextONOFF(
                context, device_number, on,
                modifierEditOn, hourForPicker, minuteForPicker,
                TimesToshow, setselectedTime
            )
            editTextOn.editTextONOFF()

            val editTextOff: EditTextONOFF = EditTextONOFF(
                context, device_number, off,
                modifierEditOff, hourForPicker,
                minuteForPicker, TimesToshow, setselectedTime)

            editTextOff.editTextONOFF()
            createHorizontalChain(
                editOn, editOff,
                chainStyle = ChainStyle.SpreadInside
            )

            val options = listOf(
                stringResource(R.string.Sun), stringResource(R.string.Mon),
                stringResource(R.string.Tue), stringResource(R.string.Wed),
                stringResource(R.string.Thu), stringResource(R.string.Fri),
                stringResource(R.string.Sat)
            )

            val selectedOptions =
                remember { mutableStateListOf(false, false, false, false, false, false, false) }

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
                            selectedDays(device_number_keeper, index, selectedOptions[index])
                        },
                        label = { options[index] }

                    )
                }
            }

        }


    }
}

class DeviceScheduler_Screen(val model: DeviceSchedulerViewModel,val context: Context,
                             val hourForPicker: Int, val minuteForPicker: Int,
                             val TimesToshow:(device_number: Int, on_or_off: String, string_onoff: String)-> String?,
                             val setselectedTime:(hourToSeT: Int,minuteToSet: Int,device_number: Int,on_or_off: String,string_onoff: String)-> String,
                             val selectedDays:(daysselected: Int, option: Int, selectedOptions: Boolean)->Unit,
                             val deviceName:(devicenbr:Int )->String) {

    val constrainWithEditText = mutableListOf<ConstrainWithEditTextOnOff>()

    @Composable
    fun deviceScheduler_Screen() {


        val devicesListSize: Int = model.deviceList()?.size ?: 0



        LazyVerticalGrid(
            columns = GridCells.Fixed(2), Modifier
                .wrapContentSize()
                .fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceEvenly,
            verticalArrangement = Arrangement.spacedBy(20.dp)
        ) {
            items(devicesListSize) {
                constrainWithEditText.add(
                    ConstrainWithEditTextOnOff(
                        context, it,
                         hourForPicker, minuteForPicker,
                        TimesToshow, setselectedTime, selectedDays, deviceName
                    )
                )
                constrainWithEditText[it].constrainWithEditTextOnOff()
            }
        }
    }
}