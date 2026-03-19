package com.example.dc_acconverterandcontrolremote
import android.Manifest
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.os.Build
import android.widget.Toast
import androidx.annotation.RequiresApi
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.wrapContentSize
import androidx.compose.material3.AlertDialog
import androidx.compose.material3.ElevatedButton
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.material3.rememberTimePickerState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.focus.onFocusChanged
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.AnnotatedString
import androidx.compose.ui.text.input.OffsetMapping
import androidx.compose.ui.text.input.TransformedText
import androidx.compose.ui.text.input.VisualTransformation
import kotlinx.coroutines.launch
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.ui.unit.sp
import androidx.core.app.ActivityCompat
import androidx.lifecycle.viewmodel.compose.viewModel
import kotlinx.coroutines.withTimeout
import kotlinx.coroutines.TimeoutCancellationException
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.delay
import java.util.Calendar

fun restartApp(context: Context) {
    val packageManager = context.packageManager
    val intent = packageManager.getLaunchIntentForPackage(context.packageName)
    val componentName = intent?.component

    // Create a new task and clear everything else
    val restartIntent = Intent.makeRestartActivityTask(componentName)
    context.startActivity(restartIntent)

    // Kill the current process to ensure a clean start
    Runtime.getRuntime().exit(0)
}

@Composable
fun TextToAddressError (addressText: String, isError: Boolean ) {
    if (isError) {
        if (addressText.isEmpty()) Text(stringResource(R.string.required))
        else Text(stringResource(R.string.more12))

    }
}
@Composable
fun TextToDeviceError (deviceText: String, isError: Boolean) {
    if (isError) {
        if (deviceText.isEmpty()) Text(stringResource(R.string.required))
        else Text(stringResource(R.string.more3))
    }
}

@Composable
fun TextToMatchFilterError (deviceText: String, isError: Boolean) {
    if (isError) {
        if (deviceText.isEmpty()) Text(stringResource(R.string.required))
        else Text(stringResource(R.string.more3))
    }
}


@OptIn(ExperimentalMaterial3Api::class)
@RequiresApi(Build.VERSION_CODES.TIRAMISU)
@Composable
fun Settings_Screen(model: DeviceSchedulerViewModel, context: Context, aware: WifiAware){

    var ipAddressText by remember { mutableStateOf(  model.IP_ADDRESS_REMOTE.toString())}
    var macAddressText by remember { mutableStateOf( model.MAC_ADDRESS_REMOTE.toString())}
    var matchFilterText by remember { mutableStateOf( model.MATCH_FILTER.toString())}
    var numberDevicesText by remember { mutableStateOf( model.NUMBER_DEVICES.toString())}
    var isBlurredIp by remember {   mutableStateOf(false)}
    var isBlurredMac by remember {  mutableStateOf(false)}
    var isBlurredDev by  remember { mutableStateOf(false)}
    var isBlurredMat by  remember { mutableStateOf(false)}
    val isErrorMac: Boolean =  ( macAddressText.length!=12) or ( macAddressText.isEmpty())
    val isErrorIp: Boolean =  ( ipAddressText.length!=12) or ( ipAddressText.isEmpty())
    val isErrorDev: Boolean = ((numberDevicesText.toIntOrNull() ?: 0) > 100) || (numberDevicesText.isEmpty())
    val isErrorMat: Boolean =  ( matchFilterText.length!=7) or ( matchFilterText.isEmpty())
    var openDialog by remember { mutableStateOf(false) }
    val scope = rememberCoroutineScope()


    Box(propagateMinConstraints = false) {

        if(openDialog){
            AlertDialog(
                onDismissRequest = {openDialog=false},
                confirmButton = {
                    TextButton(onClick = {
                        scope.launch {
                            // 1. Wait for the database/datastore save to finish
                            model.numberSetLaunch(numberDevicesText)

                            // 2. Only now, restart the app
                            restartApp(context)
                        }
                    }
                    ) {Text(stringResource(R.string.save)) }
                } ,
                dismissButton = {TextButton(onClick = {
                    openDialog=false
                }
                ) {Text(stringResource(R.string.cancel)) }
                } ,
                title ={ Text(stringResource(R.string.restartTitle)) } ,
                text = { Text(stringResource(R.string.restart)) }
            )

        }
        Column(
            modifier = Modifier
                .wrapContentSize(),
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.SpaceEvenly
        ) {
            Row(modifier = Modifier
                .wrapContentSize())
         {
             OutlinedTextField(
                 modifier = Modifier.onFocusChanged { focusState ->
                     if (!focusState.isFocused && isBlurredIp) {
                         model.IpSetLaunchLocal(ipAddressText,context )
                         isBlurredIp = false
                     }},
                 value = ipAddressText,
                 onValueChange = { newText: String ->
                     if (newText.all { it.isDigit() }) {
                         if (newText.length<13){
                         ipAddressText = newText}
                     }
                     if (!isBlurredIp) isBlurredIp = true
                 },
                 isError = isErrorIp,
                 supportingText = {TextToAddressError(ipAddressText, isErrorIp)},
                 enabled = true,
                 readOnly = false,
                 label = { Text (stringResource(R.string.ipAddress))},
                 singleLine = true,
                 visualTransformation = VisualTransformation { text ->
                     var out = ""
                     for (i in text.indices) {
                         out += text[i]
                         if (i % 3 == 2 && i < 11) out += "."
                     }
                     TransformedText(
                         text = AnnotatedString(out),
                         offsetMapping = object : OffsetMapping {
                             override fun originalToTransformed(offset: Int): Int {
                                 if (offset < 3) return offset
                                 if (offset < 6) return offset + 1
                                 if (offset < 9) return offset + 2
                                 return 15
                             }

                             override fun transformedToOriginal(offset: Int): Int {
                                 if (offset < 4) return offset
                                 if (offset < 7) return offset - 1
                                 if (offset < 10) return offset - 2
                                 return 12
                             }

                         }

                     )
                 }

             )
         }

        Row(modifier = Modifier
            .wrapContentSize())
        {


            
            OutlinedTextField(
                modifier = Modifier.onFocusChanged { focusState ->
                    if (!focusState.isFocused && isBlurredMac) {
                        model.MacSetLaunchLocal(macAddressText,context )
                        isBlurredMac = false
                    }},
                value = macAddressText,
                onValueChange = { newText: String ->
                    // Allow if empty OR if it contains valid Hex and is within length
                    if (newText.isEmpty() || (newText.all { it.isDigit() || (it in 'a'..'f') || (it in 'A'..'F') } && newText.length <= 12)) {
                        macAddressText = newText
                    }
                    if (!isBlurredMac) isBlurredMac = true
                },
                isError = isErrorMac,
                supportingText = { TextToAddressError(macAddressText, isErrorMac)},
                enabled = true,
                readOnly = false,
                label = {Text (stringResource(R.string.macAddress))},
                singleLine = true,
                visualTransformation = VisualTransformation { text ->
                    var out = ""
                    for (i in text.indices) {
                        out += text[i]
                        // Add a dot after the 3rd, 6th, and 9th characters only.
                        // Index 11 is the 12th character; we don't want a dot after it.
                        if (i % 2 == 1 && i < 10) out += "."
                    }
                    TransformedText(
                        text = AnnotatedString(out),
                        // Inside MAC OutlinedTextField VisualTransformation
                        offsetMapping = object : OffsetMapping {
                            override fun originalToTransformed(offset: Int): Int {
                                // Add +1 for every dot (at indices 2, 4, 6, 8, 10)
                                return when {
                                    offset <= 2 -> offset
                                    offset <= 4 -> offset + 1
                                    offset <= 6 -> offset + 2
                                    offset <= 8 -> offset + 3
                                    offset <= 10 -> offset + 4
                                    offset <= 12 -> offset + 5
                                    else -> 17
                                }
                            }

                            override fun transformedToOriginal(offset: Int): Int {
                                return when {
                                    offset <= 2 -> offset
                                    offset <= 5 -> offset - 1
                                    offset <= 8 -> offset - 2
                                    offset <= 11 -> offset - 3
                                    offset <= 14 -> offset - 4
                                    offset <= 17 -> offset - 5
                                    else -> 12
                                }
                            }
                        }

                    )
                }
            )
        }

            Row(modifier = Modifier
                .wrapContentSize())
            {



                OutlinedTextField(
                    modifier = Modifier.onFocusChanged { focusState ->
                        if (!focusState.isFocused && isBlurredMat && !isErrorMat) {
                            // 1. Trigger the background save to storage
                            model.setMatchFilterLaunch(matchFilterText)

                            scope.launch {
                                try {
                                    // 2. VERIFICATION: Wait up to 1 second for Storage to match UI Text
                                    // This replaces 'while' loops and ensures data integrity
                                    withTimeout(1000L) {
                                        model.discoverySettings.first { settings ->
                                            // Only proceed if what is in Storage == what the user typed
                                            settings.matchFilter == matchFilterText && settings.isReady
                                        }
                                    }
                                    delay(100)
                                        aware.sendMatchFilterToESP32(model.getMatchFilterLaunch())
                                    // 3. ACTION: Restart hardware only after data is confirmed
                                    delay(300)
                                    aware.closeSession()
                                    delay(300) // Necessary hardware "cooling" period
                                    if (ActivityCompat.checkSelfPermission(
                                            context,
                                            Manifest.permission.ACCESS_FINE_LOCATION
                                        ) != PackageManager.PERMISSION_GRANTED || ActivityCompat.checkSelfPermission(
                                            context,
                                            Manifest.permission.NEARBY_WIFI_DEVICES
                                        ) != PackageManager.PERMISSION_GRANTED
                                    ) {
                                        Toast.makeText(context, "WIFI permission failed. Try again.", Toast.LENGTH_SHORT).show()
                                    }
                                    aware.startWiFiAwareandSubscribe()

                                } catch (e: TimeoutCancellationException) {
                                    // Handle failure: Storage didn't update in time
                                    Toast.makeText(context, "Storage sync failed. Try again.", Toast.LENGTH_SHORT).show()
                                } catch (e: Exception) {
                                    println("Aware Restart failed")
                                    Toast.makeText(context, "WIFI permission failed. Try again.", Toast.LENGTH_SHORT).show()

                                }
                            }
                            isBlurredMat = false
                        }
                    },
                    value = matchFilterText,
                    isError = isErrorMat,
                    supportingText = {TextToMatchFilterError(matchFilterText, isErrorMat)},
                    onValueChange = { newText: String ->
                        if (newText.all { it.isDigit()} and (newText.length<8)) {
                            matchFilterText = newText
                        }

                        if (!isBlurredMat) isBlurredMat = true
                    },
                    label = {Text (stringResource(R.string.matchFilter))},
                    singleLine = true)
        }

        Row(modifier = Modifier
            .wrapContentSize())
        {
            OutlinedTextField(
                modifier = Modifier.onFocusChanged { focusState ->
                    if (!focusState.isFocused && isBlurredDev) {
                        // Only show the dialog if the text is different from the original model value
                        if (numberDevicesText != model.NUMBER_DEVICES.toString()) {
                            openDialog = true
                        }
                        isBlurredDev = false
                    }
                },
                value = numberDevicesText,
                onValueChange = { newText ->
                    if (newText.isEmpty()) {
                        // 1. Allow the user to clear the field (no crash!)
                        numberDevicesText = ""
                    } else {
                        // 2. Check if it's a number
                        val numericValue = newText.toIntOrNull()
                        if (numericValue != null) {
                            // 3. Only update if it's within your range (0-100)
                            if (numericValue <= 100) {

                                numberDevicesText = newText

                            }
                        }
                    }

                    // 4. Mark that the field has changed to trigger the save later
                    if (!isBlurredDev) isBlurredDev = true
                }
                ,
                isError = isErrorDev,
                supportingText = {TextToDeviceError(numberDevicesText, isErrorDev)},
                enabled = true,
                readOnly = false,
                label = {Text (stringResource(R.string.clock))},
                singleLine = true
                            )

        }
            ElevatedButton(
                onClick = {
                    model.sendTimeToWiFI(aware)
                },
                modifier = Modifier.wrapContentSize(),
                shape = MaterialTheme.shapes.extraLarge
            ) {
                Text(stringResource(R.string.clock), fontSize = 20.sp)
            }

        }
    }

}