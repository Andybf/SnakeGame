/*
 * Snake Game
 * 2023 - Anderson Bucchianico
 *
*/

let activeMenuOptionButton;
let intervalNightCycleId;

function toggleInterfaceConsole() {
    let consoleStyle = document.querySelector("#console-container").style;
    consoleStyle.display = (consoleStyle.display == '') ? 'flex' : '';
}

function clearConsole() {
    let outputConsole = document.querySelector("p#output");
    outputConsole.innerHTML = "";
}

async function initialize() {
    Module = {
        print: (function() {
            var element = document.getElementById('output');
            return function(text) {
                element.innerHTML += text + "<br>";
            };
        })(),
    
        printErr: function(text) {
            if (arguments.length > 1) {
                text = Array.prototype.slice.call(arguments).join(' ');
            }
        },
        
        canvas: (function() {
            var canvas = document.getElementById('canvas');
            return canvas;
        })()
    };

    if (window.innerWidth < 480) {
        Module.arguments = [
            `-ww${window.innerWidth}`,
            `-wh${window.innerHeight}`,
            `-th26`,
            `-tw12`,
        ];
    } else {
        Module.arguments = [
            `-ww${window.innerWidth}`,
            `-wh${window.innerHeight}`,
            `-th20`,
            `-tw20`,
        ];
    }

    let script = document.createElement("script");
    script.src = "./index.js";
    document.body.appendChild(script);
}


initialize();

