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

    // Adding Keyboard And Mouse Event Listener
    window.addEventListener("keydown", keyDown, false);
    window.addEventListener("click", mouseDown, false);
}

// Keyboard Event Listener
function keyDown(event) {

    // Code
    alert("Key Is Pressed");
}

// Mouse Event Listener
function mouseDown() {

    // Code
    alert("Mouse Button Is Clicked");
}

