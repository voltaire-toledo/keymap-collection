import sys

diagram_base = """/* ┏━━━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━┓ ┏━━━━━━━━┓
    ┃           ┃         ┃         ┃ MISSION ┃   MAC   ┃  SEARCH ┃ CTRL+ALT┃ PREVIOUS┃ PLAY/   ┃   NEXT  ┃   Vol   ┃         ┃         ┃        ┃ ┃  Knob  ┃
    ┃TAP: ESC   ┃ Bright- ┃ Bright+ ┃ CONTROL ┃ LAUNCH  ┃         ┃    + M  ┃  TRACK  ┃  PAUSE  ┃  TRACK  ┃   Mute  ┃   Vol-  ┃  Vol+   ┃   DEL  ┃ ┃  Mute  ┃
    ┃HOLD: CAPS ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃        ┃ ┗━━━━━━━━┛
    ┣━━━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━━━━┫ ┏━━━━━━━━┓
    ┃   ~   ┃    !    ┃    @    ┃    #    ┃    $    ┃    %    ┃    ^    ┃    *    ┃    -    ┃    (    ┃    )    ┃    _    ┃    +    ┃            ┃ ┃        ┃
    ┃   `   ┃    1    ┃    2    ┃    3    ┃    4    ┃    5    ┃    6    ┃    7    ┃    8    ┃    9    ┃    0    ┃    -    ┃    =    ┃    BKSP    ┃ ┃  PgUp  ┃
    ┃       ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃            ┃ ┃        ┃
    ┣━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP: TAB  ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    {    ┃    }    ┃    |    ┃ ┃        ┃
    ┃HOLD: HYPR┃    Q    ┃    W    ┃    E    ┃    R    ┃    T    ┃    Y    ┃    U    ┃    I    ┃    O    ┃    P    ┃    [    ┃    ]    ┃    \    ┃ ┃  PgDn  ┃
    ┃          ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃ ┃        ┃
    ┣━━━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP=ESC    ┃ TAP=A   ┃ TAP=S   ┃ TAP=D   ┃ TAP=F   ┃ TAP=G   ┃ TAP=H   ┃ TAP=J   ┃ TAP=K   ┃ TAP=L   ┃ TAP=;/: ┃    "    ┃                  ┃ ┃        ┃
    ┃HOLD:MO(1) ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃    '    ┃      RETURN      ┃ ┃  Home  ┃
    ┃           ┃  L_CTRL ┃  L_GUI  ┃  L_ALT  ┃ L_SHIFT ┃   MO(2) ┃   MO(2) ┃ R_SHIFT ┃  R_ALT  ┃  R_GUI  ┃  R_CTRL ┃         ┃                  ┃ ┃        ┃
    ┣━━━━━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━━━━━━━━┳━━━━━━━━┛ ┗━━━━━━━━┛
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    <    ┃    >    ┃    ?    ┃                ┃
    ┃   L_SHIFT    ┃    Z    ┃    X    ┃    C    ┃    V    ┃    B    ┃   N     ┃    M    ┃    ,    ┃    .    ┃    /    ┃      R_SHIFT   ┃ ┏━━━━━━━━━┓
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃                ┃ ┃    ↑    ┃
    ┣━━━━━━━━━━━┳━━┻━━━━━━━┳━┻━━━━━━━━━╋━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━╋━━━━━━━━━┻━━━━┳━━━━┻━━━━━┳━━━┻━━━━━━┳━━━━━━━━━┛ ┃         ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃TAP:OSL(5)┃TAP:OSL(5)┃ ┏━━━━━━━━━╋━━━━━━━━━╋━━━━━━━━┓
    ┃  L_CTRL   ┃  L_ALT   ┃   L_CMD   ┃             * < LYR_M_NAV1 SPACE *              ┃    R_CMD     ┃HOLD:MO(1)┃    ←     ┃ ┃    ←    ┃         ┃        ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃          ┃          ┃ ┃         ┃    ↓    ┃   →    ┃
    ┗━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━┛ ┗━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━┛
*/"""

file_path = r'keyboards\keychron\v1_max\ansi_encoder\keymaps\default\keymap.c'
with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Only search AFTER keymaps array declaration
array_decl = 'const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {'
start_idx = content.find(array_decl)
if start_idx == -1:
    sys.exit('keymaps array not found')

out_chunks = [content[:start_idx + len(array_decl)]]
idx = start_idx + len(array_decl)

while True:
    start = content.find('LAYOUT_ansi_82(', idx)
    if start == -1:
        out_chunks.append(content[idx:])
        break
        
    layer_name_start = content.rfind('[', idx, start)
    layer_name_end = content.find(']', layer_name_start)
    layer_name = content[layer_name_start+1:layer_name_end]
    
    p_count = 0
    in_layout = False
    keys_start = start + len('LAYOUT_ansi_82(')
    keys_end = keys_start
    
    for i in range(start + len('LAYOUT_ansi_82') - 1, len(content)):
        if content[i] == '(':
            p_count += 1
            in_layout = True
        elif content[i] == ')':
            p_count -= 1
            if in_layout and p_count == 0:
                keys_end = i
                break
                
    keys_str = content[keys_start:keys_end]
    lines = keys_str.split('\n')
    all_keys = []
    for line in lines:
        if line.strip().startswith('//'): continue
        current_key = ''
        paren_lvl = 0
        for char in line:
            if char == '(': paren_lvl += 1
            elif char == ')': paren_lvl -= 1
            if char == ',' and paren_lvl == 0:
                if current_key.strip(): all_keys.append(current_key.strip())
                current_key = ''
            else:
                current_key += char
        if current_key.strip(): all_keys.append(current_key.strip())
        
    row_counts = [15, 15, 15, 14, 13, 10]
    col_widths = [
        [22, 12, 12, 12, 10, 19, 19, 10, 10, 10, 10, 9, 9, 22, 0],
        [22, 12, 12, 12, 10, 19, 19, 10, 10, 10, 10, 9, 9, 22, 0],
        [22, 12, 12, 12, 10, 19, 19, 10, 10, 10, 10, 9, 9, 22, 0],
        [22, 12, 12, 12, 10, 19, 19, 10, 10, 10, 10, 9, 14, 0],
        [22, 12, 12, 12, 10, 19, 19, 10, 10, 10, 10, 18, 0],
        [10, 10, 12, 15, 15, 15, 10, 10, 10, 0]
    ]
    
    if len(all_keys) >= 82:
        k_idx = 0
        formatted_rows = ['        //ESC                 //f1        //f2        //f3        //f4      //f5               //f6               //f7      //f8      //f9      //f10     //f11    //f12    //DEL                 //BUTTON']
        for r, count in enumerate(row_counts):
            row_keys = all_keys[k_idx:k_idx+count]
            k_idx += count
            row_str = '        '
            for c, key in enumerate(row_keys):
                w = col_widths[r][c]
                if w > 0:
                    row_str += f'{key},'.ljust(w)
                else:
                    row_str += f'{key}'
                    if c < len(row_keys) - 1 or r < len(row_counts) - 1:
                        row_str += ','
            formatted_rows.append(row_str.rstrip())
        final_layout = 'LAYOUT_ansi_82(\n%s)' % ('\n'.join(formatted_rows))
    else:
        final_layout = 'LAYOUT_ansi_82(' + keys_str + ')'
        
    label = f'\n  /* ==================== {layer_name} ==================== */\n'
    
    out_chunks.append(content[idx:layer_name_start-1])
    out_chunks.append(label + diagram_base.strip() + '\n    [' + layer_name + '] = ' + final_layout)
        
    idx = keys_end + 1

with open(file_path, 'w', encoding='utf-8') as f:
    f.write(''.join(out_chunks))
print('Success!')
