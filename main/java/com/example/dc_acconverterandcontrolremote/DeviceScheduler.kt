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
import androidx.compose.foundation.clickable
import androidx.compose.ui.unit.sp
import java.util.Calendar
import kotlin.Int
import androidx.lifecycle.compose.collectAsStateWithLifecycle
import kotlinx.coroutines.launch

@Composable
fun TimePickerDialog(
    onDismissRequest: () -> Unit,
    confirmButton: @Composable () -> Unit,
    dismissButton: @Composable () -> Unit,
    content: @Composable () -> Unit
) {   AlertDialog(
    onDismissRequest = onDismissRequest,
    confirmButton = confirmButton,
    dismissButton = dismissButton,
    text = { content() }
)

}

@OptIn(ExperimentalMaterial3Api::class)
    @Composable
    fun EditTextONOFF(context: Context, device_number: Int,
                       on_or_off : Boolean, modifier: Modifier,
                      viewModel : DeviceSchedulerViewModel) {


    val calendar = Calendar.getInstance()
    val timePickerState = rememberTimePickerState(
        initialHour = calendar.get(Calendar.HOUR_OF_DAY),
        initialMinute = calendar.get(Calendar.MINUTE),
        is24Hour = true
    )

    val string_onoff: String =
        if (on_or_off) stringResource(R.string.ON) else stringResource(R.string.OFF)

    val timeToShow = viewModel.TimesToshow(device_number, on_or_off) ?: ""

    var showTimePicker by remember { mutableStateOf(false) }


    Box(propagateMinConstraints = false) {
        Column(
            modifier = Modifier
                .wrapContentSize(),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            showTimePicker = false

            TextField(
                readOnly = true,
                enabled = false,
                value = timeToShow,
                onValueChange = { /* ... */ },
                label = { Text(string_onoff) },
                singleLine = true,
                placeholder = { Text(stringResource(R.string.click)) },
                modifier = modifier.clickable {
                    showTimePicker = true
                }
            )


            if (showTimePicker) {
                TimePickerDialog(
                    onDismissRequest = { showTimePicker = false },
                    confirmButton = {
                        TextButton(onClick = {
                            viewModel.setselectedTime(
                                timePickerState.hour,
                                timePickerState.minute,
                                device_number,
                                on_or_off
                            )
                            showTimePicker = false
                            Toast.makeText(context, R.string.setTimeRecorded, Toast.LENGTH_SHORT)
                                .show()
                        }) { Text(stringResource(R.string.confirm)) }
                    },
                    dismissButton = {
                        TextButton(onClick = { showTimePicker = false }) {
                            Text(stringResource(R.string.cancel))
                        }
                    }
                ) {
                    TimePicker(state = timePickerState)
                }
            }
        }
    }
}

@Composable
fun DeviceControlCard( context: Context, device_number: Int, deviceName: String,
                       viewModel : DeviceSchedulerViewModel ) {


    ConstraintLayout(
        Modifier
            .wrapContentSize()
            .background(color = MaterialTheme.colorScheme.background)
    ) {

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
                start.linkTo(parent.start)
                bottom.linkTo(editOn.top, margin = 5.dp)
                end.linkTo(timeOff.start)
            }

        val modifierEditOn: Modifier = Modifier
            .constrainAs(editOn) {
                top.linkTo(timeOn.bottom, margin = 5.dp)
                bottom.linkTo(daysOfWeek.top, margin = 5.dp)
                start.linkTo(parent.start)
                end.linkTo(editOff.start)
            }

        val modifierTextOFF: Modifier = Modifier
            .constrainAs(timeOff) {
                top.linkTo(titleName.bottom, margin = 5.dp)
                bottom.linkTo(editOff.top, margin = 5.dp)
                start.linkTo(timeOn.end)
                end.linkTo(parent.end)
            }

        val modifierEditOff: Modifier = Modifier
            .constrainAs(editOff) {
                top.linkTo(timeOff.bottom, margin = 5.dp)
                bottom.linkTo(daysOfWeek.top, margin = 5.dp)
                start.linkTo(editOn.end)
                end.linkTo(parent.end)
            }


        val modifierRButtonDaysOfWeek = Modifier
            .wrapContentSize()
            .constrainAs(daysOfWeek) {
                top.linkTo(editOn.bottom, margin = 5.dp)
                bottom.linkTo(parent.bottom, margin = 5.dp)
                centerHorizontallyTo(parent)
            }


        Text(
            text = deviceName,
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

        EditTextONOFF(context, device_number, true, modifierEditOn, viewModel)

        EditTextONOFF(context, device_number, false, modifierEditOff, viewModel)

        createHorizontalChain(
            timeOn, timeOff,
            chainStyle = ChainStyle.SpreadInside
        )

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
                        viewModel.selectedDays(device_number, index, selectedOptions[index])
                    },
                    label = { options[index] }

                )
            }
        }
    }
}




@Composable
fun DeviceScheduler_Screen( context: Context,
                            viewModel : DeviceSchedulerViewModel, aware: WifiAware ) {
    // 1. Monitor the signal
    val isReady by viewModel.isInitialized.collectAsStateWithLifecycle()
    val scope = rememberCoroutineScope()

    // 2. Capture the SNAPSHOT once initialization is finished
    val devices = remember(isReady) {
        if (isReady) viewModel.allDevices.value else emptyList()
    }

    // 3. Trigger the Main() init routine
    LaunchedEffect(Unit) {
        if (!viewModel.isInitialized.value) {
            viewModel.devicesInit()
        }
    }

    if (!isReady) {
        CircularProgressIndicator()
    } else {
        // Now it's safe to use devices.size and devices[index]
        // because they won't change until the next app restart.
        Column(
            modifier = Modifier
                .wrapContentSize(),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            val text = stringResource(R.string.sendSchedulers)

            ElevatedButton(
                onClick = {
                    viewModel.sendSchedulerToWiFI(aware)
                          },
                modifier = Modifier.wrapContentSize(),
                shape = MaterialTheme.shapes.extraLarge
            ) {
                Text(text, fontSize = 20.sp)
            }

            LazyVerticalGrid(
                columns = GridCells.Fixed(2), Modifier
                    .wrapContentSize()
                    .fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceEvenly,
                verticalArrangement = Arrangement.spacedBy(20.dp),
                contentPadding = PaddingValues(
                    bottom = 100.dp // High enough gap for your Floating Button
                )
            ) {
                items(devices.size) {
                    DeviceControlCard(context, it, devices[it].device_name!!, viewModel)

                }
            }
        }
    }

}