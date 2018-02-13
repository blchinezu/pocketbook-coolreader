#ifndef SDK_COMPATIBILITY_H_INCLUDED
#define SDK_COMPATIBILITY_H_INCLUDED

// FW 5.17 changed some defines
#if defined(POCKETBOOK_PRO_FW5) && !defined(KEY_POWER)

    #define KEY_POWER IV_KEY_POWER
    #define KEY_DELETE IV_KEY_DELETE
    #define KEY_OK IV_KEY_OK
    #define KEY_UP IV_KEY_UP
    #define KEY_DOWN IV_KEY_DOWN
    #define KEY_LEFT IV_KEY_LEFT
    #define KEY_RIGHT IV_KEY_RIGHT
    #define KEY_MINUS IV_KEY_MINUS
    #define KEY_PLUS IV_KEY_PLUS
    #define KEY_MENU IV_KEY_MENU
    #define KEY_PREV IV_KEY_PREV
    #define KEY_NEXT IV_KEY_NEXT
    #define KEY_HOME IV_KEY_HOME
    #define KEY_BACK IV_KEY_BACK
    #define KEY_PREV2 IV_KEY_PREV2
    #define KEY_NEXT2 IV_KEY_NEXT2
    #define KEY_MUSIC IV_KEY_MUSIC
    #define KEY_COVEROPEN IV_KEY_COVEROPEN
    #define KEY_COVERCLOSE IV_KEY_COVERCLOSE
    #define KEY_ZOOMOUT IV_KEY_ZOOMOUT
    #define KEY_ZOOMIN IV_KEY_ZOOMIN
    #define KEY_MENU_POWER IV_KEY_MENU_POWER

    #define KEY_SHIFT IV_KEY_SHIFT
    #define KEY_LANGUAGECHANGE IV_KEY_LANGUAGECHANGE
    #define KEY_KEYBOARDCLOSE IV_KEY_KEYBOARDCLOSE

    #define KEY_0 IV_KEY_0
    #define KEY_1 IV_KEY_1
    #define KEY_2 IV_KEY_2
    #define KEY_3 IV_KEY_3
    #define KEY_4 IV_KEY_4
    #define KEY_5 IV_KEY_5
    #define KEY_6 IV_KEY_6
    #define KEY_7 IV_KEY_7
    #define KEY_8 IV_KEY_8
    #define KEY_9 IV_KEY_9

#endif

#endif
