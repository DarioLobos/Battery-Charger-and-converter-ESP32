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
@OptIn(ExperimentalMaterial3Api::class)
fun EditTextONOFF(context: Context,
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

    val timeToShow = viewModel.TimesToshowCharger(on_or_off) ?: ""

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
                            viewModel.setSelectedTimeCharger(
                                timePickerState.hour,
                                timePickerState.minute,
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
fun ChargerControlCard(context: Context,
                       viewModel : DeviceSchedulerViewModel ) {

    val deviceName: String = stringResource(R.string.chargerName)

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
                bottom.linkTo(parent.bottom, margin = 5.dp)
                start.linkTo(editOn.end)
                end.linkTo(parent.end)
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

        EditTextONOFF(context,true, modifierEditOn, viewModel)

        EditTextONOFF(context, false, modifierEditOff, viewModel)

        createHorizontalChain(
            timeOn, timeOff,
            chainStyle = ChainStyle.SpreadInside
        )

        createHorizontalChain(
            editOn, editOff,
            chainStyle = ChainStyle.SpreadInside
        )
    }
}


@Composable
fun ChargerScheduler_Screen(context: Context, viewModel: DeviceSchedulerViewModel, aware: WifiAware) {

    Column(
        modifier = Modifier
            .wrapContentSize(),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        val text = stringResource(R.string.sendSchedulers)

        ElevatedButton(
            onClick = {
                viewModel.sendSchedulerChargerToWiFI(aware)
            },
            modifier = Modifier.wrapContentSize(),
            shape = MaterialTheme.shapes.extraLarge
        ) {
            Text(text, fontSize = 20.sp)
        }

        ChargerControlCard(context, viewModel )

    }
}
