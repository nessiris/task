/*
sin_tetris_kai.c
学生証番号：345512
氏名：横井昂典
 */

/* ランキングをファイルに書き込んでスコアを保存しようとしましたがバグが直せませんでした */

#include <stdio.h>
#include <stdlib.h>
#include <eggx.h>
#include <time.h>
#define B_SIZE 20 //ミノやブロックのサイズ
#define WIDTH 660 
#define HEIGHT 540
#define Fycoor 0 //y座標の初期値
#define Fxcoor 5 //x座標の初期値


/* プロトタイプ宣言 */
void setup(); //準備処理
void title(); //タイトル画面
void drowField(); //描画処理
void decideColor(int _kind, int y, int x); //色を決める
void moveMino(); //テトリミノを一定時間ごとに落とす
void checkLine(int _ycoor); //落ちた時、行の確認　すべて揃っていたら その行を消して、pile()呼び出す
void pile(int _y); //消えた行数分、積もっていたテトリミノを落とす
void turnMino(); //テトリミノの回転の処理
void holdMino(); //テトリミノの保存機能 （落ちているテトリミノを保存し、次に使うと落ちているテトリミノと保存したテトリミノを入れ替える）
void createMino(); //テトリミノが積まれて、新しく落ちてくるの種類を決める
int checkField(int _ycoor, int _xcoor); //次の座標に移動できるかどうかの判定 （落下できるか、回転できるか、左右に移動できるか）
void overRide(int _ycoor, int _xcoor); //現在の座標xcoor ycoorを基準とした座標に現在のテトリミノ（kindを基準とする）を追加する（fieldに足す）
void clearField(int _ycoor, int _xcoor); //現在落ちているテトリミノをfieldから削除する　（現在の座標c_x c_yを基準とした座標にあるテトリミノを消す）
int endJudge(); //エンドフラグ
void endDisplay(); //終了画面
void tetris_kai(int nwin,double x,double y,int dot_size); //タイトルロゴ
void reset(); //すべての変数をリセットする
void select(); //選択画面
void rank(); //ランキング
void keyevent(); //キーイベント
void rankScan(); //ランキングの読み込み
void rankWrite(); //ランキングの書き込み

/* 変数宣言 */
int mino[7][4][4]; //このゲーム内のテトリミノのすべての種類　テキストから読み込む
int num[112]; //テキストから読み込むとき
int ranking[9]; //通常モードのランキングのスコア
int h_ranking[9]; //特殊モードのランキングのスコア
int x = 0; //横のカウンタ変数
int y = 0; //縦のカウンタ変数
int win; //ウィンドウ
int field[26][16]; //フィールド
int move_mino[4][4]; //動いているテトリミノ
int fore_mino[26][16]; //テトリミノの落下する予測地点
int kind; //現在動いているテトリミノの種類
int next_kind; //次に落ちてくるテトリミノの種類
int mode = 0; //モード選択　０：難しい　１：通常

int time_count = 0; //落下するタイミング
int _time = 0; //時間

int _line = 0; //消した行の数
int level = 0; //レベル
int level_exp = 0; //経験値 
int level_up_flag = 0; //レベルアップしたとき１
int speed = 0; //テトリミノの落下速度  
int speed_count = 0; //加速のカウンタ変数  

int score = 0; //スコア

int xcoor = Fxcoor; //現在のx座標
int ycoor = Fycoor; //現在のy座標

int hold_mino[4][4]; //ホールドされたテトリミノ
int hold_first = 0; //最初にホールド使ったかどうかのフラグ処理
int hold_use = 1; //ホールドを使ったかどうかのフラグ処理　１のとき使える　０だと使えない

int end_flag = 0; //ゲームオーバーのための変数　１になったら終了

int pose_judge = 0; //ポーズのための変数　０のときpを押すと止まる　１のときpを押すと解除

int p_end = 0; //プログラムのエンド


/* メイン文 */
int main(void) {
  
  srand((unsigned)time(NULL)); //乱数のシード

  setup(); //準備処理

  win = gopen(WIDTH, HEIGHT); //ウィンドウを開く
  winname(win, "tetris");
  gsetbgcolor(win, "black"); // 背景色を黒に 
  layer(win, 0, 1); //レイヤーの使用

  while(1) {
    
    title(); //タイトル画面
    
    createMino(); //テトリミノ種類を決定
    
    gsetnonblock(ENABLE);
    
    while(1) {
      
      gclr(win);
  
      keyevent(); //キー入力
      
      drowField();
      if(end_flag == 1) break; //エンド条件
      copylayer(win,1,0); //描画
      msleep(100); //編集点 
      time_count++; 
      _time++; 
      
      /* タイムカウントが一定数に達するとテトリミノが落下する */
      if(time_count >=  10 - speed) {
	moveMino(); //テトリミノの落下処理
	if(end_flag == 1) break; //エンド条件
	time_count = 0;	
      }
      
      /* レベルアップしたとき */
      if(level_up_flag == 1) { 
	gsetfontset(win,"-*-fixed-medium-r-normal--%d-*-*-*-*-*-*-*", 50);
	drawstr(win, 450, HEIGHT - 120, FONTSET, 0.0,  "levelup!");
	if(level % 10 == 0) drawstr(win, 450, HEIGHT - 120, FONTSET, 0.0,  "\n\nspeed up!");
	copylayer(win,1,0); //表示
	msleep(300);
	level_up_flag = 0;
      }
      
    }
    
    endDisplay(); //終了画面
    reset(); //データの初期化（スコアやタイムなどの変数の初期値に戻す）
    /* プログラムの終了 */
    if(p_end == 1) {
      rankWrite();
      return 0; 
    }
    
  }

}


/* 動いてるミノ */
void moveMino() {
  clearField(ycoor, xcoor); //現在、動いているミノを一旦消去
  /* 積もったとする */
  if(checkField(ycoor + 1, xcoor) == 1) { 
    overRide(ycoor, xcoor); //現在の座標に値を入れ直す （フィールドに追加する） 
    checkLine(ycoor); //行を確認する

    ycoor = Fycoor; //y座標を初期値に戻す
    xcoor = Fxcoor; //x座標を初期値に戻す

    end_flag = endJudge(); //ゲームオーバーか確認

    createMino(); //次のテトリミノの種類の決定
    if(hold_use == 0) hold_use = 1; //ホールド機能を使えるようにする
  }
  
  /* 落下する */
  else {
    ycoor++; //y座標を１下げる
    overRide(ycoor, xcoor); //落下した後の座標に値を入れる
  }
  
}


/* 描画処理 */
void drowField() {
  int judge = 2; //落下の予測地点のための判定
  int count = 0; //カウンタ変数

  /* 現在落下しているテトリミノがどこまで落下できるか判定*/
  while(judge == 2) {    
    clearField(ycoor, xcoor); //現在の座標のテトリミノを一旦消去
    judge = checkField(ycoor + count, xcoor); //落下できるか判定
    if(judge == 2) count++; //落下できるなら次の座標の判定へ
  }

  overRide(ycoor, xcoor); //現在の座標にテトリミノを追加する

  /* どこまで落下するか判定した後の座標を記録しておく */
  for(y = 0; y < 4; y++) {
    for(x = 0; x < 4; x++) {
      fore_mino[y + ycoor + count - 1][x + xcoor] = move_mino[y][x];
    }
  }

  /* フィールドの描画*/
  for(y = 4; y < 26; y++) { 
    for(x = 0; x < 16; x++) {
      decideColor(field[y][x], y, x); //テトリミノの色を決める
      fillrect(win, (x + 1) * 20, (25 - y) * 20, B_SIZE, B_SIZE);
    }
  }

  /* テトリミノの外枠 */
  newrgbcolor(win, 0, 0, 0);
  for(y = 4; y < 26; y++) {
    for(x = 0; x < 16; x++) {
      drawrect(win, (x + 1) * 20, (25 - y) * 20, B_SIZE, B_SIZE);
    }
  }

  /* 次のミノ と ホールド */
  for(y = 0; y < 4; y++) {
    for(x = 0; x < 4; x ++) {
      /* next */
      decideColor(mino[next_kind][y][x] * (next_kind + 1), y, x);
      fillrect(win, 320 + (x * 20), (25 - y) * 20 - 60, B_SIZE, B_SIZE); 
      
      /* hold */
      decideColor(hold_mino[y][x], y, x);
      fillrect(win, 320 + (x * 20), (25 - y) * 20 - 240, B_SIZE, B_SIZE);
    } 
  }

  /* テトリミノの落下予測地点を消す */
  for(y = 0; y < 4; y++) {
    for(x = 0; x < 4; x++) {
      fore_mino[y + ycoor + count - 1][x + xcoor] = 0;
    }
  }

  /* 文字（レベルなど） */ 
  newrgbcolor(win, 255, 255, 255);
  gsetfontset(win,"-*-fixed-medium-r-normal--%d-*-*-*-*-*-*-*", 20);

  drawstr(win, 320, HEIGHT - 55, FONTSET, 0.0,  "next");
  drawrect(win, 300, 360, B_SIZE * 6, B_SIZE * 6); 
 
  drawrect(win, 300, 180, B_SIZE * 6, B_SIZE * 6);
  drawstr(win, 320, HEIGHT - 235, FONTSET, 0.0,  "hold");

  drawstr(win, 500, HEIGHT - 300, FONTSET, 0.0,  "ホールドはZ\n\n回転はスペース\n\nポーズはP");

  drawstr(win, 300, 100, 16, 0.0, "TIME %d", _time / 10);
  drawstr(win, 300, 80, 16, 0.0, "LINE %d", _line);
  drawstr(win, 300, 60, 16, 0.0, "LEVEL %d", level);
  drawstr(win, 400, 60, 16, 0.0, "EXP %d\nspeed %d\n speed_count %d", level_exp, speed, speed_count); 

}


/* 色を決める関数 */
void decideColor(int _kind, int y, int x) {
  switch(_kind) {
    
  case 8: //壁の描画 
    newrgbcolor(win, 153, 153, 153);
    break;
    
  case 0: //何もない
    if(fore_mino[y][x] != 0) newrgbcolor(win, 204, 204, 255); //落下の予測地点
    else newrgbcolor(win, 0, 0, 0); //そうじゃないなら何もない
    break;
    
  case 1: //kind = 0 橙色
    newrgbcolor(win, 255, 153, 0);
    break;
    
  case 2: //kind = 1 青 
    newrgbcolor(win, 0, 0, 255);
    break;
    
  case 3: //kind = 2 黄緑
    newrgbcolor(win, 0, 255, 0);
    break;
    
  case 4: //kind = 3 赤
    newrgbcolor(win, 255, 0, 0);
    break;
    
  case 5: //kind = 4 紫
    newrgbcolor(win, 204, 0, 255);
    break;
    
  case 6: //kind = 5 黄
    newrgbcolor(win, 255, 255, 0);
    break;
    
  case 7: //kind = 6 水
    newrgbcolor(win, 0, 204, 255);
    break;

  default:
    newrgbcolor(win, 255, 255, 255);
    break;
  
  }

}



/* 新しいミノの作成 */
void createMino() {

  if(end_flag == 0) {
    kind = next_kind; //現在のテトリミノの種類
    next_kind = (rand() % 7); //次のテトリミノの種類
    
    for(y = 0; y < 4; y++) {
      for(x = 5; x < 9; x++) {
	field[y][x] = mino[kind][y][x - 5] * (kind + 1); //フィールドに追加　初期値に出現(見えない所　)
	move_mino[y][x - 5] = mino[kind][y][x - 5] * (kind + 1); //動かすようの配列に入れる
      }
    }

  }
  
}  


/* 着地したときに行の確認 */
void checkLine(int _ycoor) {
  int count_row = 0; //列のカウント 10溜まったら
  int count_line = 0; //消えた行数

  for(y = 23; y >= 4; y--) {    
    for(x = 2; x < 12; x++) {
      if(field[y][x] != 0) count_row++; //ブロックがするか
    }
    
    /* １行が消えたら */
    if(count_row == 10) { 
      count_line++; //カウントする
      for(x = 2; x < 12; x++) {
        field[y][x] = 0; //消えた行を消す
      }
    }
    count_row = 0; //列のカウントを初期値に戻す
  }

  if(count_line > 1) level_exp += count_line - 1; //複数の列が消された場合、経験値ボーナス

  /* 積まれた後の処理 */
  while(count_line > 0) {
    pile(23); //積もっていたテトリミノを落とす
    count_line--; //消えた行数分行う
    _line++; //消えた行数をカウント
    level_exp++; //経験値を追加

    /* レベルアップ */
    if(level_exp >= 10) { 
      level++; //レベルをあげる
      level_exp -= 10; 
      speed_count++; //スピードアップのカウント
      level_up_flag = 1; //レベルアップしたときのフラグ
    }

    /* 加速(通常モード) */
    if(speed_count == 10 && speed < 9) {
      speed += 3; 
      speed_count = 0;
    }

  }
}


/* 消えた行より上にあるミノを一旦消去 積もっていたミノを落とす */
void pile(int _y) { 
  int pile_count = 0; 

  for(y = _y; y > 3; y--) {
    for(x = 2; x < 12; x++) {
      if(field[y + 1][x] == 0) pile_count++;
    }

    /* 何も存在しない行があるなら積もっているテトリミノを落とす */
    if(pile_count == 10) { 
      for(x = 2; x < 12; x++) {
        field[y + 1][x] = field[y][x];
        field[y][x] = 0; 
      }
    }
    pile_count = 0;
  } 
}


/* 回転の処理 */
void turnMino() {
  int before_mino[4][4]; //一旦動いているテトリミノを保存

  clearField(ycoor, xcoor); //一旦現在の座標のテトリミノを消す

  for(y = 0; y < 4; y++) {
    for(x = 0; x < 4; x++) {
      before_mino[y][x] = move_mino[y][x]; //一旦保存
    }
  }
  
  for(y = 0; y < 4; y++) {
    for(x = 0; x < 4; x++) {
      move_mino[y][x] = before_mino[3-x][y]; //回転後のテトリミノにする
    }
  }

 //回転できない場合
  if(checkField(ycoor, xcoor) == 1) { //回転できるか判定
    
    for(y = 0; y < 4; y++) {
      for(x = 0; x < 4; x++) {
        move_mino[y][x] = before_mino[y][x]; //保存しておいたテトリミノに戻す
      }
    }
  }

  overRide(ycoor, xcoor); //フィールドに処理後のテトリミノを追加する
}


/* ホールドの処理 */
void holdMino() {
  int save_mino [4][4]; //一旦保存する
  
  clearField(ycoor, xcoor); //一旦現在の座標のテトリミノを消す

  /*一旦テトリミノを保存する */
  for(y = 0; y < 4; y++) {
      for(x = 0; x < 4; x++) {
        save_mino[y][x] = move_mino[y][x]; 
      }
  }
 
  /* 初めてホールドを使ったとき */
  if(hold_first == 0) {
    hold_first = 1;
    for(y = 0; y < 4; y++) {
      for(x = 0; x < 4; x++) {
        hold_mino[y][x] = save_mino[y][x];
      }
    }
    createMino(); //入れ替えるものがないので、新しいテトリミノを作る
  }
      
  /* 初めてじゃないなら */
  else {
    hold_use = 0; //ホールドできなくする
    /* 動いていたテトリミノとホールドを入れ替える */
    for(y = 0; y < 4; y++) {
      for(x = 0; x < 4; x++) {
        move_mino[y][x] = hold_mino[y][x];
        hold_mino[y][x] = save_mino[y][x];
      }
    }
  }

  /* y、xの座標を初期値に戻す */
  ycoor = Fycoor; 
  xcoor = Fxcoor; 
  /* 処理後のテトリミノをフィールドに追加する */
  for(y = 0; y < 4; y++) {
    for(x = 0; x < 4; x++) {
      field[y][x + 5] = move_mino[y][x];
    }
  }
}  


/* 消す処理 (動いているミノのみを消去) */
void clearField(int _ycoor, int _xcoor) {
  for(y = 0; y < 4; y++) {
    for(x = 0; x < 4; x++) {
      if(field[y + _ycoor][x + _xcoor] == move_mino[y][x]) {
        field[y + _ycoor][x + _xcoor] = 0;
      }
    }
  }
}


//xcoor ycoorを基準とした座標に現在の落ちているテトリミノを追加
void overRide(int _ycoor, int _xcoor) {
  for(y = 0; y < 4; y++) {
    for(x = 0; x < 4; x++) {
      field[y + _ycoor][x + _xcoor] += move_mino[y][x];
    }
  } 
}


/* 行の判定 */
int checkField(int _ycoor, int _xcoor) {
  for(y = 0; y < 4; y++) {
    for(x = 0; x < 4; x++) {
      if(move_mino[y][x] != 0 && field[y + _ycoor][x + _xcoor] != 0) return 1; //動けない場合
    }
  }
  return 2; //動ける場合
}


/* 準備処理 */
void setup() {
  FILE*fp;
  char*fname = "tetris_input.txt";
  char s[100]; 
  
  int n = 0; //テキストファイルから読み込んだデータ
  int i = 0; //カウンタ変数

  /* ランキングの読み込み */
  rankScan();

  fp = fopen(fname, "r"); //テキストファイルから読み込む
  next_kind = (rand() % 7); //プログラム起動時の最初のテトリミノの種類

  while( fgets(s, 100, fp) != NULL) {
    num[i] = atoi(s);
    i++;
  }

  fclose(fp);

  /* テキストファイルから読み込んだ */
  for(i = 0; i < 7; i++) {
    for(y = 0; y < 4; y++) {
      for(x = 0; x < 4; x++) {
	mino[i][y][x] = num[n];
	n++;
      }
    }
  }

  /* フィールド壁などの設定 */
  for(y = 0; y < 26; y++) {
    for(x = 0; x < 16; x++) {
      if( (y == 24 && x > 0 && x < 13) || (x == 1 && y != 25) || (x == 12 && y != 25) ) field[y][x] = 8;
      else field[y][x] = 0;
    }
  }
  
}


/* タイトル画面 */ 
void title() {

  tetris_kai(win, 130, 300, 10); //タイトルロゴ

  newrgbcolor(win, 255, 255, 255);
  gsetfontset(win,"-*-fixed-medium-r-normal--%d-*-*-*-*-*-*-*", 30);
  drawstr(win, 220, 100, FONTSET, 0.0,  "PLEASE KEYPRESS");
  copylayer(win,1,0); //表示
  ggetch();
  gclr(win);
  
  gsetfontset(win,"-*-fixed-medium-r-normal--%d-*-*-*-*-*-*-*", 50);
  drawstr(win, 240, 450, FONTSET, 0.0,  "操作方法");
  gsetfontset(win,"-*-fixed-medium-r-normal--%d-*-*-*-*-*-*-*", 20);
  drawstr(win, 100, 350, FONTSET, 0.0, "カーソル\n\n→：右移動\n\n←：左移動\n\n↑：一気に落下させる\n\n↓：加速");
  drawstr(win, 360, 350, FONTSET, 0.0, "キーボード\n\nスペースキー：回転\n\nZキー：ホールド機能\n\nPキー：ポーズ画面");
  gsetfontset(win,"-*-fixed-medium-r-normal--%d-*-*-*-*-*-*-*", 30);
  drawstr(win, 220, 100, FONTSET, 0.0,  "PLEASE KEYPRESS");
  
  copylayer(win,1,0); //表示
  ggetch();
  gclr(win);
  
 gsetfontset(win,"-*-fixed-medium-r-normal--%d-*-*-*-*-*-*-*", 50);
 drawstr(win, 220, 450, FONTSET, 0.0,  "ゲーム説明");
 gsetfontset(win,"-*-fixed-medium-r-normal--%d-*-*-*-*-*-*-*", 20);
 drawstr(win, 50, 350, FONTSET, 0.0,  "ゲームオーバーの条件：一番上の行にテトリミノが積まれたら\n\nレベルアップの条件：EXPが10以上溜まったら\n\nEXPのため方；1行消すと1溜まる\n\nEXPボーナス：テトリミノを消すとき一度に複数の行を消すと\n\n               ボーナスポイントがEXPに追加");
 gsetfontset(win,"-*-fixed-medium-r-normal--%d-*-*-*-*-*-*-*", 30);
 drawstr(win, 220, 100, FONTSET, 0.0,  "PLEASE KEYPRESS");
 
 copylayer(win,1,0); //表示
 ggetch();
 gclr(win);

 select(); //選択画面

 gclr(win); 

 newrgbcolor(win, 255, 255, 255);
 gsetfontset(win,"-*-fixed-medium-r-normal--%d-*-*-*-*-*-*-*", 50);
 drawstr(win, 200, 300, FONTSET, 0.0,  "GAME START");
 copylayer(win,1,0); //表示
 
 msleep(500);
 
}


/* ゲームエンドフラグ*/
int endJudge() {
  y = 4; //一番上の行
  for(x = 2; x < 12; x++) {
    if(field[y][x] != 0) return 1; //終了
  }
   return 0;
}


/* 終了画面 */
void endDisplay() {
  score = (level + 1) * _line;
  gclr(win);

  newrgbcolor(win, 255, 0, 0);
  gsetfontset(win,"-*-fixed-medium-r-normal--%d-*-*-*-*-*-*-*", 100);
  drawstr(win, 125, 300, FONTSET, 0.0,  "GAMEOVER");

  newrgbcolor(win, 255, 255, 255);
  
  gsetfontset(win,"-*-fixed-medium-r-normal--%d-*-*-*-*-*-*-*",50);
  drawstr(win, 110, 200, FONTSET, 0.0, "SCORE %d", score);
  
  gsetfontset(win,"-*-fixed-medium-r-normal--%d-*-*-*-*-*-*-*",30);
  drawstr(win, 220, 100, FONTSET, 0.0,  "PLEASE KEYPRESS");
  copylayer(win,1,0); //表示
  gsetnonblock(DISABLE);
  ggetch();
  msleep(1000);
  gclr(win);
  rank(); //ランキング
  msleep(1000);
}


/* 杉原君のドットエディタを借りました */
void tetris_kai(int nwin,double x,double y,int dot_size) {
	int qi;
	int qj;
	int dot_data[38][9][4] = {{{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{0,0,255,1},{0,0,255,1}},{{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{0,0,255,1},{0,0,255,1}},{{0,0,255,1},{0,0,255,1},{0,0,255,1},{0,0,255,1},{0,0,255,1},{0,0,255,1},{0,0,255,1},{0,0,255,1},{0,0,255,1}},{{0,0,255,1},{0,0,255,1},{0,0,255,1},{0,0,255,1},{0,0,255,1},{0,0,255,1},{0,0,255,1},{0,0,255,1},{0,0,255,1}},{{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{0,0,255,1},{0,0,255,1}},{{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{0,0,255,1},{0,0,255,1}},{{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0}},{{0,255,0,1},{0,255,0,1},{0,255,0,1},{0,255,0,1},{0,255,0,1},{0,255,0,1},{0,255,0,1},{0,255,0,1},{0,255,0,1}},{{0,255,0,1},{0,255,0,1},{0,255,0,1},{0,255,0,1},{0,255,0,1},{0,255,0,1},{0,255,0,1},{0,255,0,1},{0,255,0,1}},{{0,255,0,1},{0,255,0,1},{255,255,255,0},{255,255,255,0},{0,255,0,1},{0,255,0,1},{255,255,255,0},{0,255,0,1},{0,255,0,1}},{{0,255,0,1},{0,255,0,1},{255,255,255,0},{255,255,255,0},{255,255,255,0},{0,255,0,1},{255,255,255,0},{0,255,0,1},{0,255,0,1}},{{0,255,0,1},{0,255,0,1},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{0,255,0,1}},{{0,255,0,1},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0}},{{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,153,0,1},{255,153,0,1}},{{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,153,0,1},{255,153,0,1}},{{255,153,0,1},{255,153,0,1},{255,153,0,1},{255,153,0,1},{255,153,0,1},{255,153,0,1},{255,153,0,1},{255,153,0,1},{255,153,0,1}},{{255,153,0,1},{255,153,0,1},{255,153,0,1},{255,153,0,1},{255,153,0,1},{255,153,0,1},{255,153,0,1},{255,153,0,1},{255,153,0,1}},{{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,153,0,1},{255,153,0,1}},{{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,153,0,1},{255,153,0,1}},{{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0}},{{204,0,255,1},{204,0,255,1},{204,0,255,1},{204,0,255,1},{204,0,255,1},{204,0,255,1},{204,0,255,1},{204,0,255,1},{204,0,255,1}},{{204,0,255,1},{204,0,255,1},{204,0,255,1},{204,0,255,1},{204,0,255,1},{204,0,255,1},{204,0,255,1},{204,0,255,1},{204,0,255,1}},{{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{204,0,255,1}},{{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{204,0,255,1},{255,255,255,0},{255,255,255,0},{204,0,255,1}},{{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{204,0,255,1},{204,0,255,1},{204,0,255,1},{255,255,255,0},{204,0,255,1}},{{255,255,255,0},{255,255,255,0},{255,255,255,0},{204,0,255,1},{204,0,255,1},{204,0,255,1},{255,255,255,0},{204,0,255,1},{204,0,255,1}},{{255,255,255,0},{255,255,255,0},{204,0,255,1},{204,0,255,1},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{204,0,255,1}},{{255,255,255,0},{204,0,255,1},{204,0,255,1},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0}},{{204,0,255,1},{204,0,255,1},{255,255,255,0},{0,204,255,1},{0,204,255,1},{0,204,255,1},{0,204,255,1},{255,255,255,0},{0,204,255,1}},{{204,0,255,1},{255,255,255,0},{0,204,255,1},{0,204,255,1},{0,204,255,1},{0,204,255,1},{0,204,255,1},{255,255,255,0},{0,204,255,1}},{{255,255,255,0},{0,204,255,1},{0,204,255,1},{0,204,255,1},{0,204,255,1},{0,204,255,1},{255,255,255,0},{255,255,255,0},{255,255,255,0}},{{255,0,0,1},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,0,0,1},{255,255,255,0}},{{255,0,0,1},{255,0,0,1},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,0,0,1},{255,0,0,1},{255,0,0,1}},{{255,0,0,1},{255,0,0,1},{255,255,255,0},{255,255,255,0},{255,0,0,1},{255,0,0,1},{255,0,0,1},{255,0,0,1},{255,0,0,1}},{{255,0,0,1},{255,0,0,1},{255,255,255,0},{255,0,0,1},{255,0,0,1},{255,0,0,1},{255,255,255,0},{255,0,0,1},{255,0,0,1}},{{255,0,0,1},{255,0,0,1},{255,0,0,1},{255,0,0,1},{255,0,0,1},{255,255,255,0},{255,255,255,0},{255,0,0,1},{255,0,0,1}},{{255,0,0,1},{255,0,0,1},{255,0,0,1},{255,0,0,1},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,0,0,1},{255,0,0,1}},{{255,0,0,1},{255,0,0,1},{255,0,0,1},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,255,255,0},{255,0,0,1}}};
	for(qi=0;qi<38;qi++) {
		for(qj=0;qj<9;qj++) {
			if(dot_data[qi][qj][3] == 1) {
				newrgbcolor(nwin,dot_data[qi][qj][0],dot_data[qi][qj][1],dot_data[qi][qj][2]);
				fillrect(nwin,x+(double)(qi*dot_size),y+(double)(qj*dot_size),dot_size,dot_size);
			}
		}
	}
}


/* 選択画面 */
void select() {
  int _y = 1; //プログラムを終了するかタイトルに戻るか
  int command;

  gsetfontset(win,"-*-fixed-medium-r-normal--%d-*-*-*-*-*-*-*", 50);
  drawstr(win, 220, 460, FONTSET, 0.0,  "モード選択");
  
  newrgbcolor(win, 255, 255, 255);
  gsetfontset(win,"-*-fixed-medium-r-normal--%d-*-*-*-*-*-*-*", 50);
  drawstr(win, 130, 370, FONTSET, 0.0,  "normal");
  drawstr(win, 130, 150, FONTSET, 0.0,  "special"); 
  gsetfontset(win,"-*-fixed-medium-r-normal--%d-*-*-*-*-*-*-*", 20);
  drawstr(win, 160, 320, FONTSET, 0.0,  "時間は無制限です。\n\n１０レベル上がる毎にスピードが上がります。\n\n四段階に分かれています");
  drawstr(win, 160, 100, FONTSET, 0.0,  "こちらも時間は無制限です。\n\nしかし、いきなり最高速度です。\n\n"); 
  drawstr(win, 450, 10, FONTSET, 0.0,  "決定はENTERです");  
  copylayer(win,1,0); //表示
  
  while(1) {
    
    /* カーソル */ 
    for(x = 0; x < 4; x++) {
      newrgbcolor(win, 0, 0, 255);
      fillrect(win, x * B_SIZE, 150 + 220 * _y, B_SIZE, B_SIZE);
    }
    
    copylayer(win,1,0); //表示
    
    command = ggetch(); 

    if(command == 30) { //上　通常
      _y = 1; 
      newrgbcolor(win, 0, 0, 0);
      fillrect(win, 0, 150, B_SIZE * 4, B_SIZE);
    }
    
    if(command == 31) { //下 特殊
      _y = 0; 
      newrgbcolor(win, 0, 0, 0);
      fillrect(win, 0, 370, B_SIZE * 4, B_SIZE);
    }
    
    copylayer(win,1,0); //表示

    if(command == 13) {  //enter
      
      switch(_y) {
      case 0:
	speed = 9; //スピードを最高速度に
	mode = 0;
	break;

      case 1:
	speed = 0;
	mode = 1;
	break;
      }

      break;
    }

  }
  
}


/* リセット（変数の初期化）*/
void reset() { 
  gclr(win);
  xcoor = Fxcoor; //x座標
  ycoor = Fycoor; //y座標
  _time = 0; //時間
  time_count = 0; //テトリミノの落下の速度の決定
  speed = 0; //速度
  speed_count = 0; //速度をあげるときのカウント
  level = 0; //レベル
  _line = 0; //消した行数
  level_exp = 0; //経験値 
  level_up_flag = 0; //レベルアップのフラグ
  score = 0; //スコア
  hold_first = 0; //ホールド機能を最初に使った
  hold_use = 1; //ホールド機能の制御
  end_flag = 0; //ゲームオーバーかのフラグ
  pose_judge = 0; //ポーズのフラグ
  
  /* フィールドの初期化 */
  for(y = 0; y < 24; y++) {
    for(x = 2; x < 12; x++) {
      field[y][x] = 0;
    }
  }

  /* 配列の初期化*/
  for(y = 0; y < 4; y++) {
    for(x = 0; x < 4; x++) {
      fore_mino[y][x] = 0; //落下の予測地点
      move_mino[y][x] = 0; //動いているテトリミノ
      hold_mino[y][x] = 0;  //ホールドしたテトリミノを保存
    }
  }
  next_kind = (rand() % 7); //次のテトリミノの種類を決定

}


/* ランキング画面 */
void rank() {
  int _ranking = 0; //一時的保存
  int count = 0; //カウンタ変数
  //scoreは現在のプレーヤー
  newrgbcolor(win, 255, 255, 255);

  /* 通常 */
  if(mode == 1) { 

    /* 最下位より大きいなら */
    if(score >= ranking[8]) {
      ranking[8] = score; 
      
      /* 順位の入れ替え*/
      for(count = 7; count >= 0; count--) {
	if(ranking[count] <= score) {
	  _ranking = ranking[count];
	  ranking[count] = score;
	  ranking[count + 1] = _ranking;
	}
      }
    }
    gsetfontset(win,"-*-fixed-medium-r-normal--%d-*-*-*-*-*-*-*", 50);
    drawstr(win, 220, 450, FONTSET, 0.0,  "ランキング");
    gsetfontset(win,"-*-fixed-medium-r-normal--%d-*-*-*-*-*-*-*", 20);
    drawstr(win, 100, 400, FONTSET, 0.0,  "１位：%d\n\n２位：%d\n\n３位：%d\n\n４位：%d\n\n５位：%d\n\n６位：%d\n\n７位：%d\n\n８位：%d\n\n９位：%d", ranking[0], ranking[1], ranking[2], ranking[3], ranking[4], ranking[5], ranking[6], ranking[7], ranking[8]);
    drawstr(win, 220, 100, FONTSET, 0.0,  "ENTERを押したらタイトル画面に戻ります\n\nSPACEを押したらゲーム終了");
  }
  
  /* 難しい */
  else if(mode == 0) { 

    /* 最下位より大きいなら */
    if(score >= h_ranking[8]) {
      h_ranking[8] = score; 

      /* 順位の入れ替え*/
      for(count = 7; count >= 0; count--) {
	if(h_ranking[count] <= score) {
	  _ranking = h_ranking[count];
	  h_ranking[count] = score;
	  h_ranking[count + 1] = _ranking;
	}
      }
    }
    gsetfontset(win,"-*-fixed-medium-r-normal--%d-*-*-*-*-*-*-*", 50);
    drawstr(win, 220, 450, FONTSET, 0.0,  "ランキング");
    gsetfontset(win,"-*-fixed-medium-r-normal--%d-*-*-*-*-*-*-*", 20);
    drawstr(win, 100, 400, FONTSET, 0.0,  "１位：%d\n\n２位：%d\n\n３位：%d\n\n４位：%d\n\n５位：%d\n\n６位：%d\n\n７位：%d\n\n８位：%d\n\n９位：%d", h_ranking[0], h_ranking[1], h_ranking[2], h_ranking[3], h_ranking[4], h_ranking[5], h_ranking[6], h_ranking[7], h_ranking[8]);
    drawstr(win, 220, 100, FONTSET, 0.0,  "ENTERなどを押したらタイトル画面に戻ります\n\nSPACEを押したらゲーム終了");
  }
  
  copylayer(win,1,0); //表示
  if(ggetch() == 32) p_end = 1; //プログラムの終了
  gclr(win);
}


/*キー入力*/
void keyevent() {

 int code, type;
 float tempx, tempy;

  if( ggetxpress( &type, &code, &tempx, &tempy) == win ) {
    if(type == KeyPress) {
      
      /* 右 */
      if(code == 28) { 
	clearField(ycoor, xcoor); //現在、動いているミノを一旦消去
	
	/* 移動できる場合 */
	if(checkField(ycoor, xcoor + 1) == 2) { //移動できるか判定
	  xcoor++;
	  overRide(ycoor, xcoor); //移動後の座標に追加
	}

	/* 移動できない場合 */
	else {
	  overRide(ycoor, xcoor); //現在の座標に追加する（もとに戻す）
	}
      }

      /* 左 */
      if(code == 29 && xcoor > 0) { 
	clearField(ycoor, xcoor); //現在、動いているミノを一旦消去

	/* 移動できる場合 */
	if(checkField(ycoor, xcoor - 1) == 2) { //移動できるか判定
	  xcoor--;
	  overRide(ycoor, xcoor); //移動後の座標に追加
	}
	
	/* 移動できない場合 */
	else {
	  overRide(ycoor, xcoor); //現在の座標に追加する（もとに戻す）
	}
      }
      
      /* 上 一気に落とす */
      if(code == 30) {
	int judge = 2; //落下の予測地点の判定
	int count = 0; //カウンタ変数

	clearField(ycoor, xcoor); //現在の座標のテトリミノを一旦消去  

	/* どこまで落下できるか調べる */
	while(judge == 2) { 
	  judge = checkField(ycoor + count, xcoor); //落下できるか判定
	  /* 落下できる場合 */
	  if(judge == 2) {
	    count++; //次の座標へ
	  }
	}

	overRide(ycoor + count - 1, xcoor); //処理後の座標に追加
	checkLine(ycoor); //行を確認
	end_flag = endJudge(); //ゲームオーバーか判定
	if(hold_use == 0) hold_use = 1; //ホールド機能を使えるようにする
	ycoor = Fycoor; //y座標を初期値に戻す
	xcoor = Fxcoor; //x座標を初期値に戻す
	createMino(); //次のテトリミノの種類の決定
      }

      /* 下 */
      if(code == 31) moveMino(); //テトリミノを能動的に落下させる

      /* 回転 */
      if(code == 32) turnMino(); //SPACE 32

      /* ホールド機能 */
      if( (code == 122) && hold_use == 1) holdMino(); //z = 122
      
      /* ポーズ機能 */
      if(code == 112) { //p = 112
	gsetnonblock(DISABLE); //ノンブロッキングモードの解除
	newrgbcolor(win, 255, 255, 255);
	gsetfontset(win,"-*-fixed-medium-r-normal--%d-*-*-*-*-*-*-*", 30);
	drawstr(win, 145, 300, FONTSET, 0.0,  "ポーズ中\n\nキーボードを押して下さい");
	copylayer(win,1,0); //表示
	ggetch();
	gsetnonblock(ENABLE); //ノンブロッキングモードにする
	gclr(win);
      }
    }
  }
}


/* ランキングの読み込み */ 
void rankScan() {
  int i;
  char filename[] = "rank.txt";
  FILE *fp;
  int _rank[20];
  char s[100];
	
  //"w" = write
  //"r" = read
  //"a" = add
  fp = fopen(filename, "r");

  if(fp == NULL) {
    fp = fopen(filename, "w");
    for(i = 0; i < 20; i++) {
      fprintf(fp, "0\n");
    }
  }

  else {
    i = 0;
    while( fgets(s, 100, fp) != NULL) {
      _rank[i] = atoi(s);
      i++;
    }

    for(i = 0; i < 20; i++) {
      if(i < 10) ranking[i] = _rank[i];
      else h_ranking[i] = _rank[i];
      printf("_rank[%d] %d\n", i, _rank[i]);
    }
    
    fclose(fp);
  }
  
}


/* ファイルの書き込み */
void rankWrite() {
  int i; //カウンタ変数
  char filename[] = "rank.txt";
  FILE *fp;

  fp = fopen(filename, "w");
  for(i = 0; i < 20; i++) {
    if(i < 10) {
      fprintf(fp, "%d\n", ranking[i]);
      //printf("i = %d  %d\n", i, ranking[i]);
    }
    
    else {
      fprintf(fp, "%d\n", h_ranking[i]);
      //printf("i = %d  %d\n", i, h_ranking[i]);
    }
  }
  fclose(fp);
}



