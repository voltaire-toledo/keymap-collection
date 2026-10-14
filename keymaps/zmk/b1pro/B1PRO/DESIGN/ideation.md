# Ideas to leverage

## Layer key:

#### One tap = momentary layer, double tap = lock into that layer.

A Glove80 ZMK keymap does exactly this with &mo then &to.
Great for a non-critical layer key. Glove80 keymap (https://gist.github.com/vineethk/fd68f9c1230902323cd28a89ef850edb)

#### Bluetooth profile key: tap selects a known device/profile; double tap enters discovery/pairing.

This is excellent for your output/Bluetooth controls, since it keeps daily use simple and hides pairing behind intent. Glove80 Bluetooth tap-dance example. (https://gist.github.com/vineethk/fd68f9c1230902323cd28a89ef850edb)

#### Caps “escalator”: tap = temporary Shift layer, double = Caps Word, triple = real Caps Lock.

It gives programming identifiers and acronyms a pleasant path without dedicating multiple keys. Advantage360 ZMK keymap (https://gist.github.com/robkisk/89cfde34687e02e84e565033d38652d8)

#### Editor command pairs: tap = Paste, double = Copy; another key tap = browser Back, double = Forward.

This works best on a Nav/utility layer, where the visual or positional pairing makes the dance memorable. Advantage360 ZMK keymap (https://gist.github.com/robkisk/89cfde34687e02e84e565033d38652d8)

#### Coding punctuation: tap (, double tap ).

The same idea applies to [], {}, and quote pairs.
> **Keymap Strategy Rule:** *Shortcut key sequences should not exceed the number of steps without a shortcut.*
> Multi-tap dances should not exceed 2 taps; 3-step sequences (e.g. open + close + left arrow) belong on dedicated momentary macro layers instead of triple-tap dances.

#### Smart mode key: tap = one-shot Numpad/Symbol picker, double = lock that layer, triple = return to Base.

This is powerful, but I’d keep it away from the factory Fn key at first.

For your key immediately left of the arrows, I’d use a custom hold-tap-not tap dance-as the safe first experiment:

- Tap → one-shot layer-picker.
- Hold → preserve MO(1) exactly, so Fn+- bootloader and Fn+J+Z reset remain intact.

Then reserve tap dance for safer keys: Caps, a punctuation key, or Bluetooth/output controls. I would not assign bootloader/reset to
a multi-tap gesture.

