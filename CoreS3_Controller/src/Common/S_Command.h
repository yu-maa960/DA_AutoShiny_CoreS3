#pragma once

enum class Sword_DA_Command {
    A,  //Aボタン連打
    CHOOSE, //「一人で選択」
    FINISH, //持ち帰るポケモンを選ぶ場面
    PAUSE,  //暗転待機
    SHINY,  //色違い感知
    NONE,   //何もなし
};

enum class S_Controller_Command {
    UP,
    DOWN,
    RIGHT,
    LEFT,
    RESET,
    A,
    B,
    X,
    Y,
};