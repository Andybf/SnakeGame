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

function clearConsole(event) {
    let outputConsole = document.querySelector("p#output");
    outputConsole.innerHTML = "";
}

function mixColor(base, topColor, weight) {
    return {
         r: base.r * (1-weight) + topColor.r * weight,
         g: base.g * (1-weight) + topColor.g * weight,
         b: base.b * (1-weight) + topColor.b * weight
    };
}

var Module = {
    arguments: [
        `-ww${window.innerWidth}`,
        `-wh${window.innerHeight}`,
    ],
    print: (function() {
        var element = document.getElementById('output');
        return function(text) {
            element.innerHTML += text + "<br>";
        };
    })(),
    printErr: function(text) {
            if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
            if (0) {
                dump(text + '\n');
            }
    },
    canvas: (function() {
        var canvas = document.getElementById('canvas');
        return canvas;
    })()
};
                       
updateInterfaceColor(calcInterfaceColorFactor(720));
