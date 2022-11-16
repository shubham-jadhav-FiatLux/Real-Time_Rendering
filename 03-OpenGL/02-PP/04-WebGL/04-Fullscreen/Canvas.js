// JavaScript source code
var canvas = null;
var context = null;
var bFullscreen = false;

function main() {

    // Code
    // Get Canvas
    canvas = document.getElementById("SBJ");
    if (!canvas)
        console.log("Obtaining Canvas Failed !\n");
    else
        console.log("Obtaining Canvas Succeeded.\n");

    // Get 2D Context From Canvas
    context = canvas.getContext("2d");
    if (!context)
        console.log("Obtaining Context Failed !\n");
    else
        console.log("Obtaining context Succeeded.\n");

    // Paint The Background Of Canvas By Black Color
    context.fillStyle = "black";                        // In place of black we may right its hexadecimal code as "#000000".
    context.fillRect(0, 0, canvas.width, canvas.height);

    drawText("Hello, World !!!");

    // Adding Keyboard And Mouse Event Listener
    window.addEventListener("keydown", keyDown, false);
    window.addEventListener("click", mouseDown, false);
}

function drawText(text) {

    // code
    // Declare The String
    var str = text;

    // Decide The Font
    context.font = "48px sans-serif";

    // Decide Text Color
    context.fillStyle = "white";                        // In place of white we may right its hexadecimal code as "#FFFFFF".

    // Put The Text In The Middle Of The Canvas Horizontally And Vertically
    context.textAlign = "center";                       // This Is Horizontal Center
    context.textBaseline = "middle";                    // This Is Vertical Center

    // Display The Text
    context.fillText(str, canvas.width / 2, canvas.height / 2);
}

function toggleFullscreen() {

    // Code
    var fullscreen_element = document.fullscreenElement ||
                             document.mozFullScreenElement ||
                             document.webkitFullscreenElement ||
                             document.msFullscreenElement ||
                             null;

    if (fullscreen_element == null) {

        if (canvas.requestFullscreen)
            canvas.requestFullscreen();
        else if (canvas.mozRequestFullScreen)
            canvas.mozRequestFullScreen();
        else if (canvas.webkitRequestFullscreen)
            canvas.webkitRequestFullscreen();
        else if (canvas.msRequestFullscreen)
            canvas.msRequestFullscreen();

        bFullscreen = true;
    }
    else {

        if (document.exitFullscreen)
            document.exitFullscreen();
        else if (document.mozExitFullScreen)
            document.mozExitFullScreen();
        else if (document.webkitExitFullScreen)
            document.webkitExitFullScreen();
        else if (document.msExitFullScreen)
            document.msExitFullScreen();

        bFullscreen = false;
    }
}

// Keyboard Event Listener
function keyDown(event) {

    // Code
    switch (event.keyCode) {

        case 70:
            toggleFullscreen();
            //drawText("Hello, World !!!");
            break;
    }
}

// Mouse Event Listener
function mouseDown() {

    // Code
}

