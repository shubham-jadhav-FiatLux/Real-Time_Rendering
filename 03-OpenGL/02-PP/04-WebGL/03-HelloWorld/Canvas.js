// JavaScript source code
function main() {

    // Code
    // Get Canvas
    var canvas = document.getElementById("SBJ");
    if (!canvas)
        console.log("Obtaining Canvas Failed !\n");
    else
        console.log("Obtaining Canvas Succeeded.\n");

    // Get 2D Context From Canvas
    var context = canvas.getContext("2d");
    if (!context)
        console.log("Obtaining Context Failed !\n");
    else
        console.log("Obtaining context Succeeded.\n");

    // Paint The Background Of Canvas By Black Color
    context.fillStyle = "black";                        // In place of black we may right its hexadecimal code as "#000000".
    context.fillRect(0, 0, canvas.width, canvas.height);

    // Declare The String
    var str = "Hello, World !!!";

    // Decide The Font
    context.font = "48px sans-serif";

    // Decide Text Color
    context.fillStyle = "white";                        // In place of white we may right its hexadecimal code as "#FFFFFF".

    // Put The Text In The Middle Of The Canvas Horizontally And Vertically
    context.textAlign = "center";                       // This Is Horizontal Center
    context.textBaseline = "middle";                    // This Is Vertical Center

    // Display The Text
    context.fillText(str, canvas.width / 2, canvas.height / 2);

    // Adding Keyboard And Mouse Event Listener
    window.addEventListener("keydown", keyDown, false);
    window.addEventListener("click", mouseDown, false);
}

// Keyboard Event Listener
function keyDown(event) {

    // Code
}

// Mouse Event Listener
function mouseDown() {

    // Code
}

