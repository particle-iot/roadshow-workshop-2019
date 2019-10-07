# Chapter 3: Using Particle Webhooks with IFTTT, and exploring on-device debugging

| **Project Goal**            | Use Particle Webhooks and Integrations to connect your app to IFTTT; Learn to perform on-device debugging in Particle Workbench.                                                                                        |
| --------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **What you’ll learn**       | Working with Particle Integrations, IFTTT, and on-devie debugging in Particle Workbench |
| **Tools you’ll need**       | Particle Workbench, an [IFTTT.com](https://ifttt.com) account, a Particle Argon, and the Grove Starter Kit for Particle Mesh, a Particle Debugger.                                                                                                            |
| **Time needed to complete** | 60 minutes                                                                                                                                                                |

In this session, you're going to explore the power of Particle integrations with the popular IF This Then That (IFTTT) service, then explore using Particle Workbench to perform on-device debugging. If you get stuck at any point during this session, [click here for the completed, working source](https://go.particle.io/shared_apps/5d40aec2279e1e000b9ad57b).

First, let's take a brief look at the Particle CLI and Device Cloud API.

## Exploring the Particle CLI and Device Cloud API

### The Particle CLI

1.  If you haven't already, [install the Particle CLI](https://docs.particle.io/guide/tools-and-features/cli/photon/). Open a terminal window and type the following command:

```bash
bash <( curl -sL https://particle.io/install-cli )
```

2.  Type `particle login` and enter your Particle account email and password when prompted.

![](./images/04/particlelogin.gif)

3.  Once you're logged in, type `particle list` to see your list of online devices.

![](./images/04/particlelist.gif)

4.  The device you've been using for this workshop has two variables and one function. Get the value of the `temp` variable with the command `particle get temp`.

![](./images/04/temp.gif)

5.  You can also call one of the two functions to light up the yellow or blue LED button. Type the command `particle call <your-device-name> toggleLed` in the terminal. Run the same command again to turn the light off.

### The Particle Device Cloud API

Behind the scenes, every interface that Particle provides to work with devices, from the Console, to mobile apps, SDKs, and the CLI, they all talk through a RESTful Device Cloud API. You can even call yourself, directly.

_The next few steps assume you have cURL installed on your machine. If you don't have this command-line utility on your machine, you can download and install it [here](https://curl.haxx.se/download.html) or use a GUI-based tool like [Postman](https://www.getpostman.com/)._

1.  First, you'll need to obtain an access token from the CLI. Type `particle token list` to view the tokens associated with your account. The first one listed is your `user` token, and can be used to access the Device Cloud API. If no tokens are listed, generate a new one with `particle token new`.

2.  With your token and Device ID in hand, type the following cURL command into a terminal window, replacing the text below in `< >` with your information.

```bash
curl https://api.particle.io/v1/devices?access_token=<your token>
```

By default, the response will generate a wall of text in your terminal. If you have Python 2.6+ installed on your machine, you can pipe the output to the `json.tool` and get pretty-printed JSON.

```bash
curl https://api.particle.io/v1/devices\?access_token\=<your token>
| python -m json.tool
```

![](./images/04/curllist.gif)

3.  For this next command, you need the Device ID of the Photon attached to your badge. You can find that in the console or via the `particle list` CLI command.

4.  Let's call the `toggleLed` function using the Device Cloud API. Type the following, again replacing the text below in `< >` with your information.

```bash
curl https://api.particle.io/v1/devices/<device id>/toggleB \
     -d access_token=<your token>
```

![](./images/04/curlcall.gif)

You've now explored a number of ways that you can interface with the Particle Device cloud and your connected devices! Now, let's go beyond the Particle ecosystem and explore some of the ways that you can integrate with other 3rd party services, and backhaul your data into other cloud services.


## Introducing the Particle Integrations and IFTTT

### Setting up an IFTTT Integration

IFTTT (If This, Then That) is a web-based service that allows you to create integrations via simple conditional statements, called applets. There are hundreds of pre-built services you can leverage, and first-class support for Particle devices. In this section, you're going to create an IFTTT integration that posts a tweet when you press a button on your badge.

**Note**: During the flow below, you'll need to connect both your Particle and Google accounts with IFTTT. When prompted, follow the on-screen instructions to do so.

1.  Start by heading over to [IFTTT](https://ifttt.com/particle) and either login, or create a new account.

![](./images/04/ifttt.png)

2.  After logging in, click your profile picture in the top right of your dashboard and select the "Create" menu option.

![](./images/04/newapplet.png)

3.  Click the "+This" button.

![](./images/04/ifthis.png)

4.  In the "Search services" input, type "particle" and click on the Particle card.

![](./images/04/chooseservice.png)

![](./images/04/chooseparticle.png)

5.  Click on the "New event published" card.

![](./images/04/choosetrigger.png)

6. In the trigger fields, set the event name as `env-vals`, leave the Event Content field blank and set the Device name to the name of your device. Click "create trigger."

![](./images/04/triggerfields.png)

7. You've set-up the trigger on the Particle end, now its time for the **That**+ portion of the setup. Click the "+That" button.

![](./images/04/thenthat.png)

8. In the "Search services" input, type "Google Sheets" and click on the Google Sheets card.

![](./images/04/choose-gsheets.png)

9. Click on the "Add row to spreadsheet" card.

![](./images/04/addrow-card.png)

10. In the action fields, set the spreadsheet name to "TCEnvVals." Leave the defaults in the other fields and click "Create action."

![](./images/04/ifttt-gsheets.png)

11. Click the "Finish" button to create your applet.

![](./images/04/ifttt-reviewpng.png)

![](./images/04/ifttt-live.png)

Now, let's modify our device firmware to publish temp and humidity values.

### Publishing a payload with temp and humidity values

Now, let's send the current temp, humidity and light level using a `Particle.publish` event. You'll sind a single event with all three values in a single JSON object. To do this, you'll use the `JSONParserGenerator` library.

1. Open the Workbench Command Palette (CMD+SHIFT+P or CTRL+SHIFT+P) and select the "Particle: Install Library" option.

2. In the text box, type "JsonParserGeneratorRK" and click enter.

3. At the top of your project, add an include statement:

```cpp
#include "JsonParserGeneratorRK.h"
```

4. Add a new function to your app called `createEventPayload` that takes the temp, humidity and light readings. This function will create an instance of `JsonWriterStatic` and `JsonWriterAutoObject` objects, insert key-value pairs for each reading, and then get the JSON object as a string buffer that you can send along in a `Particle.publish()` event.

```cpp
void createEventPayload(int temp, int humidity, double light)
{
  JsonWriterStatic<256> jw;
  {
    JsonWriterAutoObject obj(&jw);

    jw.insertKeyValue("temp", temp);
    jw.insertKeyValue("humidity", humidity);
    jw.insertKeyValue("light", light);
  }
}
```

5. Now, let's publish a new event, and call the `createEventPayload` function to provide a formatted JSON string for the data parameter. Add the following to the end of your `createEventPayload` function.

```cpp
Particle.publish("env-vals", jw.getBuffer(), PRIVATE);
```

6. Finally, your `loop` function, call the `createEventPayload` function you just created.

```cpp
createEventPayload(temp, humidity, currentLightLevel);
```

### Posting sensor values to Google Sheets

1. Flash this firmware to your device and navigate to the Particle console. Every few seconds, you'll see a new `env-vals` event show up.

![](./images/04/env-vals.png)

2. Open Google Drive and look for a folder named "events." In that folder, you'll find a Sheet called "TCEnvVals." Open it, and you'll see a row for each event published by your device:

![](./images/04/gsheet.png)

Now that you have data streaming into Google Sheets, let's transform the data and create some simple visualizations!

### Processing data in Google Sheets

Before you create data visualizations with our sensor data, you need to transform the sensor values into discrete values. You'll do this by creating a simple script that processes the raw data as it is added to the main sheet, inserting each raw sensor value in a new sheet for data viz.

1. In the "TCEnvVals" document, create a new sheet called "Processed" and give it four columns, "Temp," "Humidity," "Light" and "Time."

2. In the Tools menu, click the "Script Editor" option, which will open a new tab with the Google Apps Script editor.

3. Click on the "Untitled project" text and give the project a name, like "ProcessTCSensorData."

4. Remove the default function code and add an `onChange` event with the following code.

```js
function onChange(e) {
  var row = SpreadsheetApp.getActiveSheet().getLastRow();
  
  var envVals = SpreadsheetApp.getActiveSheet().getRange("B" + row).getValue();
  var time = SpreadsheetApp.getActiveSheet().getRange("D" + row).getValue();
  
  var envObj = JSON.parse(envVals);
  
  if (envObj.temp != '0') {
    var ss = SpreadsheetApp.getActiveSpreadsheet();
    var processedSheet = ss.getSheetByName('Processed');
    processedSheet.appendRow([envObj.temp, envObj.humidity, envObj.light, time]);
  }
}
```

The code above is a JavaScript function that uses the Google Sheets API to get the last row inserted into the main sheet, extract the sensor vals, and the timestamp. The sensor vals are parsed into a JSON object, and then you add a new row to the "Processed" sheet with those values and the timestamp.

5. Save the file.

6. Now, you'll need to add a change trigger to this app. Click the clock icon to open the triggers for this project.

![](./images/04/opentriggers.png)

7. Click "Add Trigger" at the bottom left.

![](./images/04/addtrigger.png)

8. In the trigger window that opens, make sure that `onChange` is selected as the function name, and "On change" is selected as the event type.

![](./images/04/addtriggerwindow.png)

9. Click save to create the trigger. You'll be asked to sign-in with your Google Account and grant access to your app. Follow the on-screen instructions to do so.

![](./images/04/triggerlist.png)

10. In the tab for your spreadsheet, select the "Processed" sheet. After a few moments, records should start coming through. 

![](./images/04/processed.png)

If you're not seeing anything after a bit, you can click the "Check now" button in your IFTTT Applet.

![](./images/04/checknow.png)

### Visualizing data with Google Sheets

Once you have some processed data, you can add a chart to your sheet!

1. Create a new tab called "DataViz"

2. Click the "Insert" menu and select "Chart."

![](./images/04/chart.png)

3. The Chart menu will open on the right side of the window. In the "Chart type" drop-down, select the combination chart type.

![](./images/04/combochart.png)

4. Click the X-Axis box to open the "Select a data range" window. Navigate to the "Processed" sheet and select the top of the Time tab.

![](./images/04/selectX.gif)

5. Click "Add Series" to open the "Select a data range" window.

![](./images/04/addtemp.gif)

6. Repeat the same process to add Series data for the "Humidity" and "Light" columns.

![](./images/04/chartfilled.png)

7. Now let's customize. In the Chart editor, click the "Customize" tab and expand the "Series" item. Select the "Temp" series. Then, change the type to "Line."

![](./images/04/templine.png)

8. Now select the "Light" series and change the type to "Columns."

![](./images/04/lightline.png)

9. Close the chart editor. If everything has been configured properly, you'll see a chart that looks like this.

![](./images/04/finalchart.png)

And that's how you do DataViz with Google Sheets and IFTTT. Now performing on-device debugging with Particle Workbench.

## On-Device Debugging with Workbench

So far in this workshop, we've used the USB Serial interface to inspect and debug the current state of our devices with `Serial.print()` statements. And while this approach is handy for quickly inspecting the state of a running application, Particle provides first-class on-device debugging support with Particle Workbench. In this section, we'll explore using Workbench to step-debug our Particle-powered app.

### Prepare for Debugging

In order to perform step debugging and a few other features, you'll need to disable Mesh and BLE support in your application firmware. This is to address a current limitation in software that DeviceOS depends on to handle real-time networking and communication.

At the start of your `setup` function, add the following two lines to disable Mesh and BLE during debugging. The `#if` preprocessor ensures that these commands are only compiled into the binary if you're running a debug build. 

```cpp
#if defined(DEBUG_BUILD)
Mesh.off();
BLE.off();
#endif
```

Now, continue onto the next step. When you flash a debug build to your device, you'll automatically disable Mesh and BLE so you can perform step debugging. 


### Connect the Particle Debugger

To complete this portion of the lab, you'll need the following:

- Particle Workbench
- The trusty Argon you've been using for other parts of this lab
- One [Particle Debugger](https://store.particle.io/products/particle-debugger)
- A free USB port on your laptop

Both the Debugger and the Particle device must be connected to the same computer via USB, so make sure you have enough ports available. If your laptop has only USB C ports available, ask a lab proctor for an adapter.

Connect the debugger to the debug port on your Particle device using the provided ribbon cable, and plug both into your computer. The debug port consists of ten exposed pins protected by some plastic, and you can find it next to the LiPo battery port near the top of the device.

![](./images/03/DebuggerCable.png)

### Create a debug firmware binary

Once everything is connected and powered on, you’ll need to put your Particle device into Device Firmware Update (DFU) mode. In order for Workbench to facilitate on-device debugging, it needs to compile and flash a debug build of your application and the Particle Device OS firmware in a single binary. You’ll sometimes see this referred to as a “monolithic” build because this differs from the normal build process, where Device OS and your application firmware can be updated independent of one another.

1. To put your device in DFU mode, press and hold the `MODE` and `RESET` buttons on the device. Then, release the `RESET` button, while continuing to hold the `MODE` button until the RGB LED on the device starts blinking yellow.

![](./images/03/DFU.gif)

2. Now that you’re setup on the hardware side, let’s start a debugging session and cover some Workbench basics. Make sure your project from this workshop is open in Workbench. Click the "bug" icon in the VS Code / Workbench sidebar to open the debug sidebar.

3. Click the debug configurations dropdown and select the "Particle debugger (argon, boron/bsom, xenon)" option.

![](./images/03/DebugMenu.png)

4. Finally, click the green arrow next to the debug configuration dropdown to initiate the debug process.

![](./images/03/GreenArrow.png)

Now, get ready to wait a bit as Workbench creates a debug binary for your project. This will go much faster on repeat runs, but the first run is a good time to grab a cup of coffee, take a stretch break, or [practice your sword-fighting in the hallway](https://xkcd.com/303/).

5. Once the debug binary is built and flashed to your device, Workbench will halt and your device will power down, which you’ll see once the onboard RGB LED turns off. You’ll also see the following message in the Debug Console, which is expected.

![](./images/03/DebugConsole.png)

6. Click the continue button to power your device back up. Once you’re breathing cyan again, you’re ready to debug!

![](./images/03/ContinueButton.png)

### Step-debug your device with Workbench

#### Setting breakpoints and stepping through code

1. Let's start with one of the most commonly-used debug features: setting breakpoints. This allows you to pause and inspect the state of running code during execution. You can set breakpoints by clicking in the gutter next to an individual line, or from the “Breakpoints” section of the debug sidebar. 

2. Click on the gutter next to the line that reads the current temp from your device. When a breakpoint is set, a red circle will appear next to the line on which to break. 

3. Since you set your first breakpoint inside of the `loop`, it will be hit automatically after the interval period passes. You'll know that execution is paused because the line you set a breakpoint on will be highlighted in yellow and an arrow will appear in the gutter next to that line.

![](./images/03/break.png)

4. Once a breakpoint is hit, the line in question will be highlighted in yellow. From here, you can use the Debug menu at the top of the screen to step through your code. From left to right in the image below, those buttons allow you to continue, step over the current line, step into the current line, step out of the current scope, restart the debug session (you’ll need to put the device back in DFU mode for this to work) and finally, to disconnect the session.

![](./images/03/debugmenu2.png)

5. Press the continue button. Your project will resume execution and break again on teh same line after the interval delay elapses.

6. It's also possible to only break code when a condition you define is met. Start by setting another breakpoint a few lines later on the `createEventPayload` line.

7. Right click on the gutter next to that line and select the "Add conditional breakpoint" option.

8. In the expression window that expands under the line you selected, you can enter a simple expression to evaluate each time this line is reached. When the expression evaluates to true, the breakpoint will halt execution. Otherwise, execution will continue as normal. Enter the expression `temp > 68` and hit Enter.

![](./images/03/conditional.png)

9. Hit the continue button again. This time, execution will stop on your conditional breakpoint. Try changing the conditional value to `temp > 90` and hit the continue button again. Observe that, this time, the breakpoint is not hit.

#### Inspecting variables

In addition to step-debugging to inspect the execution of our apps, we often want to inspect the state of certain variables and objects. Workbench provides two ways to do this: Either by hovering over a variable in the code window, or by viewing variables in the "Variables" panel of the debug sidebar.

1. While debugging and paused on any line, hover your mouse over the `currentLightLevel` variable. A tooltip will appear showing the current value of the variable.

2. To view all variables, expand the "Variables" panel of the debug sidebar. This panel shows information about all local, global and static variables currently in scope at the point of execution.

![](./images/03/variablessidebar.png)

#### Watching variable values

Beyond inspecting local and global variables, you can use the watch panel to tell the debugger to keep track of the state of a given variable or object. This can be quite useful if you want to see how a portion of your application mutates or is affected by another variable, or if you want to take a closer look at when a given variable or object comes into or goes out of scope as your app is running.

1. To watch a variable, expand the "Watch" panel of the debug sidebar. Click the "+" symbol, and add `jw` in the expression box. 

![](./images/03/jw.png)

2. Now, add a breakpoint to the first line in the `createEventPayload` function, which creates the `JsonWriterStatic` object.

![](./images/03/jwbreakpoint.png)

3. Start (or continue) debugging until you reach the breakpoint you just set. Hit the step-over icon (second from the left) and watch how the `jw` value in the Watch window changes. 

4. Hit the step-over icon a few more times until you reach the end of the `createEventPayload` function, and observe how the Watch value changes.

#### Navigating the call stack

Finally, let's look at using the debugger to navigate the call stack.

1. Start by adding the breakpoint to the first line of the `toggleLed` function. If you've stopped debugging, restart and continue until your device is breathing cyan.

![](./images/03/callstackbreakpoint.png)

2. Open the console and call the `toggleLed` Cloud function using the `Call` button on  your device dashboard screen. The breakpoint you set on the `toggleLed` function will be hit shortly after you call the function.

4. Expand the "Call Stack" panel of the Debug sidebar. This view shows you the entire call stack for your application, from the Device OS logic that handled calling your function, all the way up to the main program loop. You can click on any item in the stack list to view the source and current execution location of the program.

![](./images/03/incallstack.png)

#### Advanced debugging features

In addition to the features covered in this lab, the Workbench debugger provides access to several debug features built-in to VS Code and leveraged by the cortex-debug extension, including the ability to inspect and edit hardware registers, peripherals, and even direct memory locations. All of these are outside of the scope of this workshop, so [check out the docs](https://code.visualstudio.com/docs/editor/debugging) for these tools to learn more about how to use them.

### Stepping out of the debug cycle

Once you’re done debugging and ready to resume normal coding and flashing, you’ll need to get your device out of its debug state, meaning that you want to replace the “monolithic” debug build with the hybrid application and device OS firmware. To do this, put the device back into DFU mode and run the `Particle: Flash application & DeviceOS (local)` command in the Workbench command palette.

And if you ever run into weird errors while debugging or flashing, the Workbench clean commands are your friends! In our experience, when these things pop up, running `Particle: Clean application & DeviceOS (local)` and `Particle: Clean application for debug (local)` are usually enough to set things right again.

<hr/>

**Congratulations! You've completed this workshop. Now you're a Particle Master! Take a moment to pat yourself on the back and bask in your newfound IoT-commandery. And if you want to take your exploration further, click the "Extra" link below!**


**BEFORE YOU GO** you'd love it if you could provide feedback on this workshop. Please visit [this link](https://particleiot.typeform.com/to/JiF8xM) to rate your experience.
