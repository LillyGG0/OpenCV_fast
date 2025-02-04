#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>

#define GRID_SIZE 10
#define CELL_SIZE 100
#define WIDTH (GRID_SIZE * CELL_SIZE)
#define HEIGHT (GRID_SIZE * CELL_SIZE)

typedef struct nodes {
    int cost;
    int heuristic;
    int total_cost;
    bool available;
    bool passed;
    int x, y;
    struct nodes* parent;
} Nodes;

IplImage* img;
Nodes node[GRID_SIZE][GRID_SIZE];
Nodes* start_node;
Nodes* goal_node;

bool show_path = false;  // 経路表示フラグ

int heuristic(Nodes a, Nodes b);
void Setcost(Nodes* a, int cost, int heuristic);
void mouseHandler(int event, int x, int y, int flags, void* param);
void a_star(Nodes* start_node,Nodes* goal_node);
void drawPath(Nodes* node);


int main(void) {
    img = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 3);
    cvNamedWindow("A* Pathfinding");
    cvSetMouseCallback("A* Pathfinding", mouseHandler, NULL);

    // ノードの初期化
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            node[i][j].x = j;
            node[i][j].y = i;
            node[i][j].available = true;
            node[i][j].passed = false;
            node[i][j].parent = NULL;
        }
    }
    start_node = &node[0][0];
    goal_node = &node[GRID_SIZE - 1][GRID_SIZE - 1];

    // フォント設定
    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0, 0, 2, 8);

    int key = 0;
    while (key != 27) {  // ESCキーで終了
        cvSet(img, cvScalar(255, 255, 255));  // 背景を白色に設定

        // グリッド線を描画 (黒色)
        for (int i = 0; i <= GRID_SIZE; i++) {
            cvLine(img, cvPoint(0, i * CELL_SIZE), cvPoint(WIDTH, i * CELL_SIZE), cvScalar(0, 0, 0), 1);
            cvLine(img, cvPoint(i * CELL_SIZE, 0), cvPoint(i * CELL_SIZE, HEIGHT), cvScalar(0, 0, 0), 1);
        }

        // 障害物を描画
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                if (!node[i][j].available) {
                    cvRectangle(img,
                        cvPoint(j * CELL_SIZE, i * CELL_SIZE),
                        cvPoint((j + 1) * CELL_SIZE, (i + 1) * CELL_SIZE),
                        cvScalar(0, 0, 255), CV_FILLED);
                }
            }
        }

        // スタートとゴールの文字を描画
        cvCircle(img, cvPoint(50, 50), 50, cvScalar(0, 255, 0), -1);
        cvCircle(img, cvPoint(WIDTH - 50, HEIGHT - 50), 50, cvScalar(0, 255, 0), -1);

        // 経路を描画
        if (show_path) {
            drawPath(goal_node);
        }

        cvShowImage("A* Pathfinding", img);

        key = cvWaitKey(10);
        if (key == 13) {  // Enterキー
            for (int i = 0; i < GRID_SIZE; i++) {
                for (int j = 0; j < GRID_SIZE; j++) {
                    node[i][j].parent = NULL;
                }
            }
            a_star(start_node, goal_node);
            show_path = true;
        }
    }

    cvReleaseImage(&img);
    cvDestroyWindow("A* Pathfinding");
    return 0;
}

int heuristic(Nodes a, Nodes b) {//ヒューリスティックを返す関数
    int heuristic = (abs(a.x - b.x)) + (abs(a.y - b.y));
    return heuristic;
}

void Setcost(Nodes* a, int cost, int heuristic) {//ノードのコストを設定する関数
    a->cost = cost;
    a->heuristic = heuristic;
    a->total_cost = cost + heuristic;
    return;
}


// A*アルゴリズム
void a_star(Nodes* start_node,Nodes*goal_node) {


    Nodes* open_nodes[GRID_SIZE * GRID_SIZE] = {nullptr};
    //Nodes* close_nodes[GRID_SIZE * GRID_SIZE];
    Nodes* current_node = start_node;//現在のノードをスタートノードで初期化
    int open_number = 0;//オープンされたノードの数(オープンリストに入っているノードの数)
    Setcost(current_node, 0, heuristic(*current_node, *goal_node));//現在のノード(スタートノード)の値を設定

    open_nodes[0] = current_node;//オープンリストの先頭に現在のノードを追加
    open_number++;//open_numberを1増やす

    int roopcount = 0;
    while (open_number > 0 && current_node != goal_node) {//オープンリストにノードがあるかぎり探索を続ける
        roopcount++;
        int mincost_node_index = 0;//最小コストのノードのインデックスを保持する変数
        for (int i = 0; i < open_number - 1; i++) {//for文でopen_nodes内を比較し、最小コストを探す
                if (open_nodes[mincost_node_index]->total_cost > open_nodes[i]->total_cost) {//最小コストノードの総コストi番目のノードの総コストを比較
                    mincost_node_index = i;//i番目のノードの総コストがより小さければ、indexを更新
                }
                else if(open_nodes[mincost_node_index]->total_cost == open_nodes[i]->total_cost){
					//総コストが同じなら
					if (open_nodes[mincost_node_index]->cost > open_nodes[i]->cost) {//コストを比較
						mincost_node_index = i;//コストがより小さければ、indexを更新
					}

                }
        }

        Nodes* next_node = open_nodes[mincost_node_index];//最小コストのノードを次に移動するノードとする

        //close_nodes[close_number] = current_node;//現在のノードをクローズリストに追加
        current_node->passed = true;//通行済みにする
        open_nodes[mincost_node_index] = open_nodes[open_number - 1];//最小コストノードをオープンリストから削除(リストの最後尾のノードで上書きして"強引に","実質的"な削除をしている)
        open_number--;//通過したノードはもう探索しないので、open_numberをひとつ減らす
        //next_node->parent = current_node;//移動先のノードの親ノードを現在のノードに設定
        
        current_node = next_node;//現在のノードを移動先のノードに更新

       

        if (current_node == goal_node)return;//この時点でゴールノードに到達しているなら探索を終了する

        int new_cost = current_node->cost + 1;


        int neighbors_index[4][2] = { {1,0},{-1,0},{0,1},{0,-1} };
        for (int i = 0; i < 4; i++) {
            Nodes* neighbor;
            int neighbor_x = current_node->x + neighbors_index[i][0];
            int neighbor_y = current_node->y + neighbors_index[i][1];
            bool x_is_normal = (neighbor_x >= 0 && neighbor_x < 10);
            bool y_is_normal = (neighbor_y >= 0 && neighbor_y < 10);
            if (x_is_normal && y_is_normal) {
                neighbor = &node[neighbor_y][neighbor_x];
            }
            else
            {
                continue;
            }
            bool already_open = false;
            if (!neighbor->passed && neighbor->available) {
                for (int j = 0; j < open_number - 1; j++) {//オープンリストをforで探す

                    if (open_nodes[j] == neighbor) {//aがすでにオープンリストにあるなら
                        already_open = true;
                        break;
                    }

                }

                if (!already_open) {//already_openがfalse(未オープン)だったなら
                    open_nodes[open_number] = neighbor;//オープンリストに入れる
                    Setcost(neighbor, new_cost, heuristic(*neighbor, *goal_node));
                    neighbor->parent = current_node;
                    open_number++;//オープンしたノードの数を1増やす
                }
            }
        }

    }

}


// 経路を描画
void drawPath(Nodes* node) {
    while (node->parent != NULL) {
        cvLine(img,
            cvPoint(node->x * CELL_SIZE + CELL_SIZE / 2, node->y * CELL_SIZE + CELL_SIZE / 2),
            cvPoint(node->parent->x * CELL_SIZE + CELL_SIZE / 2, node->parent->y * CELL_SIZE + CELL_SIZE / 2),
            cvScalar(0, 255, 0), 2, 8);
        node = node->parent;
    }
}

// マウスコールバック関数
void mouseHandler(int event, int x, int y, int flags, void* param) {
    int grid_x = x / CELL_SIZE;
    int grid_y = y / CELL_SIZE;

    if (grid_x < GRID_SIZE && grid_y < GRID_SIZE) {
        if (event == CV_EVENT_LBUTTONDOWN) {  // 左クリック: 障害物を追加
            node[grid_y][grid_x].available = false;
        }
        else if (event == CV_EVENT_RBUTTONDOWN) {  // 右クリック: 障害物を削除
            node[grid_y][grid_x].available = true;
        }
    }
}
        }
    }
}
