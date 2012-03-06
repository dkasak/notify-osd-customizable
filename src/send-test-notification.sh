#!/bin/sh

notify-send "Take note" "The next example will test the icon-only layout-case" -i dialog-info
sleep 2
notify-send "Eject" -i notification-device-eject -h string:x-canonical-private-icon-only:
sleep 2
notify-send "WiFi signal found" -i notification-network-wireless-medium
sleep 2
notify-send "WiFi signal lost" -i notification-network-wireless-disconnected
sleep 2
notify-send "Volume" -i notification-audio-volume-medium -h int:value:75 -h string:x-canonical-private-synchronous:
sleep 2
notify-send "Volume" -i notification-audio-volume-low -h int:value:30 -h string:x-canonical-private-synchronous:
sleep 2
notify-send "Brightness" -i notification-display-brightness-high -h int:value:101 -h string:x-canonical-private-synchronous:
sleep 2
notify-send "Brightness" -i notification-keyboard-brightness-medium -h int:value:45 -h string:x-canonical-private-synchronous:
sleep 2
notify-send "Testing markup" "Some <b>bold</b>, <u>underlined</u>, <i>italic</i> text. Note, you should not see any marked up text."
sleep 2
notify-send "Jamshed Kakar" "Hey, what about this restaurant? http://www.blafasel.org

Would you go from your place by train or should I pick you up from work? What do you think?"
sleep 2
notify-send "English bubble" "The quick brown fox jumps over the lazy dog." -i network
sleep 2
notify-send "Bubble from Germany" "Polyfon zwitschernd aßen Mäxchens Vögel Rüben, Joghurt und Quark." -i gnome-system
sleep 2
notify-send "Very russian" "Съешь ещё этих мягких французских булок, да выпей чаю." -i dialog-info
sleep 2
notify-send "More from Germany" "Oje, Qualm verwölkt Dix zig Farbtriptychons." -i gnome-globe
sleep 2
notify-send "Filter the world 1/3" "<a href=\"http://www.ubuntu.com/\">Ubuntu</a>
Don't rock the boat
Kick him while he&apos;s down
\"Film spectators are quiet vampires.\"
Peace &amp; Love
War & Peace
Law &#38; Order
Love &#x26; War
7 > 3
7 &gt; 3"
sleep 2
notify-send "Filter the world 2/3" "7 &#62; 3
7 &#x3e; 3
14 < 42
14 &lt; 42
14 &#60; 42
14 &#x3c; 42
><
<>
< this is not a tag >
<i>Not italic</i>"
sleep 2
notify-send "Filter the world 3/3" "<b>So broken</i>
<img src=\"foobar.png\" />Nothing to see
<u>Test</u>
<b>Bold</b>
<span>Span</span>
<s>E-flat</s>
<sub>Sandwich</sub>
<small>Fry</small>
<tt>Testing tag</tt>"
