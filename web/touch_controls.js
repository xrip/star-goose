// Touch controls in the manner of a mobile action game: a floating stick under
// the left thumb, an arc of actions under the right one. They only set the
// bits the game already reads. The page waits for landscape before the game
// sizes its canvas.

var gooseTouch = (function () {
    var ICONS = {
        fire: '<path d="M12 2l1.6 6.2H20l-5 3.8 1.9 6L12 14.8 7.1 18l1.9-6-5-3.8h6.4z"/>',
        left: '<path d="M16 5L7 12l9 7M7 12h11"/>',
        right: '<path d="M8 5l9 7-9 7M17 12H6"/>'
    };
    var BITS = { left: 0x01, right: 0x02, up: 0x04, down: 0x08, rightDevice: 0x20, leftDevice: 0x40, fire: 0x80 };
    var root = document.createElement("div");
    root.id = "touch";
    root.innerHTML =
        '<div id="touch-stick"><div class="stick-base"><div class="stick-knob"></div></div></div>' +
        '<div id="touch-actions"></div>';
    document.body.appendChild(root);

    var stickBits = 0;
    var held = {};

    function vibrate() {
        if (navigator.vibrate) navigator.vibrate(10);
    }

    function refresh() {
        var mask = stickBits;
        for (var id in held) mask |= held[id].bit;
        Module.touchMask = mask;
    }

    function addButton(parent, name, bit, label) {
        var button = document.createElement("button");
        button.type = "button";
        button.className = name;
        button.tabIndex = -1;
        button.setAttribute("aria-label", label);
        button.innerHTML = '<svg viewBox="0 0 24 24">' + ICONS[name] + "</svg>";
        button.addEventListener("pointerdown", function (event) {
            if (event.button !== 0) return;
            event.preventDefault();
            event.stopPropagation();
            button.setPointerCapture(event.pointerId);
            held[event.pointerId] = { bit: bit, button: button };
            button.classList.add("down");
            refresh();
            vibrate();
        });
        function release(event) {
            var press = held[event.pointerId];
            if (!press || press.button !== button) return;
            delete held[event.pointerId];
            button.classList.remove("down");
            refresh();
        }
        button.addEventListener("pointerup", release);
        button.addEventListener("pointercancel", release);
        button.addEventListener("lostpointercapture", release);
        parent.appendChild(button);
    }

    var actions = root.querySelector("#touch-actions");
    addButton(actions, "left", BITS.leftDevice, "Левое орудие");
    addButton(actions, "right", BITS.rightDevice, "Правое орудие");
    addButton(actions, "fire", BITS.fire, "Огонь");
    setupStick(root.querySelector("#touch-stick"));

    function showTouch() {
        document.body.classList.add("touch");
    }
    if (matchMedia("(pointer: coarse)").matches) showTouch();
    window.addEventListener("touchstart", showTouch, { passive: true, once: true });

    function setupStick(zone) {
        var radius = 56;
        var deadZone = 0.25;
        var base = zone.querySelector(".stick-base");
        var knob = zone.querySelector(".stick-knob");
        var pointer = null;
        var centerX = 0;
        var centerY = 0;

        function aim(x, y) {
            var dx = x - centerX;
            var dy = y - centerY;
            var force = Math.hypot(dx, dy) / radius;
            var clamp = Math.min(1, force ? 1 / force : 0);
            knob.style.transform = "translate(" + (dx * clamp) + "px," + (dy * clamp) + "px)";
            if (force < deadZone) {
                stickBits = 0;
                refresh();
                return;
            }
            var angle = Math.round(Math.atan2(dy, dx) / (Math.PI / 4)) * (Math.PI / 4);
            var sx = Math.round(Math.cos(angle));
            var sy = Math.round(Math.sin(angle));
            var bits = 0;
            if (sx < 0) bits |= BITS.left;
            if (sx > 0) bits |= BITS.right;
            if (sy < 0) bits |= BITS.up;
            if (sy > 0) bits |= BITS.down;
            stickBits = bits;
            refresh();
        }

        function release(event) {
            if (event.pointerId !== pointer) return;
            pointer = null;
            stickBits = 0;
            refresh();
            base.removeAttribute("style");
            knob.removeAttribute("style");
            base.classList.remove("active");
        }

        zone.addEventListener("pointerdown", function (event) {
            event.preventDefault();
            if (pointer !== null) return;
            pointer = event.pointerId;
            zone.setPointerCapture(pointer);
            var box = zone.getBoundingClientRect();
            centerX = event.clientX;
            centerY = event.clientY;
            base.style.left = (centerX - box.left) + "px";
            base.style.top = (centerY - box.top) + "px";
            base.classList.add("active");
            aim(centerX, centerY);
            vibrate();
        });
        zone.addEventListener("pointermove", function (event) {
            if (event.pointerId === pointer) aim(event.clientX, event.clientY);
        });
        zone.addEventListener("pointerup", release);
        zone.addEventListener("pointercancel", release);
        zone.addEventListener("lostpointercapture", release);
    }

    var portraitPhone = matchMedia("(orientation: portrait) and ((pointer: coarse) or (hover: none))");

    function lockLandscape() {
        if (screen.orientation && screen.orientation.lock) {
            screen.orientation.lock("landscape").catch(function () {});
        }
    }

    return {
        lock: lockLandscape,
        // The game fixes its canvas when it starts, so a phone must be turned first.
        landscape: function () {
            return new Promise(function (resolve) {
                function check() {
                    if (portraitPhone.matches) return;
                    portraitPhone.removeEventListener("change", check);
                    requestAnimationFrame(function () { resolve(); });
                }
                portraitPhone.addEventListener("change", check);
                check();
            });
        }
    };
})();
