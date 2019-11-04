//
// Created by lart on 2019/11/4.
//

#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;

const int PACK_MAX_W = 80;                  //背包最大承受重量
const int PACK_MAX_V = 80;                  //背包最大承受容积
const int NUM = 32;                         //物品数
const int MAX_GENERATION = 100;             //遗传最大代数
const int PS = 500;                         //种群规模，相当于就是包含的个体数量
const float PC = 0.8;                       //交叉率
const float PV = 0.1;                       //变异率


const int zl[NUM] = {22, 15, 4, 5, 10, 19, 21, 20, 8, 13, 2, 3, 6, 17, 12, 5, 12, 4, 12, 21, 14, 23, 17, 15, 20, 22, 25,
                     0, 22, 15, 25, 13};
const int tj[NUM] = {11, 22, 12, 21, 21, 13, 1, 10, 13, 8, 6, 25, 13, 27, 12, 23, 12, 24, 23, 11, 6, 24, 28, 10, 20, 13,
                     25, 23, 5, 26, 30, 15};
const int value[NUM] = {8, 9, 15, 6, 16, 9, 1, 4, 14, 9, 3, 7, 12, 4, 15, 5, 18, 5, 15, 4, 6, 2, 12, 14, 11, 9, 13, 13,
                        14, 13, 19, 4};

//随机产生01
int pp() {
    float p;
    p = rand() % 1000 / 1000.0;
    if (p < 0.8)
        return 0;
    else
        return 1;
}

//个体类
class Entity {
public:
    int fit;        // 个体的适应度
    int sum_w;      // 个体的总重量  个体基因编码对应的物品的总重量
    int sum_v;      // 个体的总体积  个体基因编码对应的物品的总体积
    int sum_val;    // 个体的总价值  个体基因编码对应的物品的总价值
    int gene[NUM];  // gene使用NUM位二进制编码 是否存放对应的物品
    int _count;

    Entity() {
        fit = 0;
        sum_v = 0;
        sum_w = 0;
        sum_val = 0;
        int i;
        for (i = 0; i < NUM; i++)
            gene[i] = 0;
    }

};

//遗传算法类
class GA {
private:
    Entity zq[PS];                          //种群
    Entity max_single;                      //最优个体

public:
    //初始化种群
    void Init();

    //计算个体价值重量
    int Cal_SingleValue(int row);

    int Cal_SingleW(int row);

    int Cal_SingleV(int row);

    //计算个体适应度
    void Cal_Fitness();

    //计算价值最大个体，这里的 _generation 表示对应的演化到了第几代
    void Cal_Maxval_Single(int _generation);

    //选择
    void Select();

    //是否交叉
    bool IsCross() {
        return ((rand() % 1000 / 1000.0) <= PC);
    }

    //交叉
    void Cross();

    //是否变异
    bool IsVariation() {
        return ((rand() % 1000 / 1000.0) <= PV);
    }

    //变异
    void Variation();

    //进行遗传，每五代几率变异
    void Run() {
        Init();

        for (int i = 0; i < MAX_GENERATION; i++) {
            Cal_Fitness();
            Cal_Maxval_Single(i);
            Select();
            Cross();
            if (i % 5 == 0 && i != 0) {
                Variation();
            }
        }

        Cal_Fitness();
        Cal_Maxval_Single(MAX_GENERATION);
        cout << "The best value is:" << max_single.fit << endl;
        cout << "The best entity's gene is:" << endl;

        for (int i = 0; i < NUM; i++) {
            cout << max_single.gene[i];
            if (i != NUM - 1)
                cout << ",";
        }

        cout << endl << "The best entity is in the " << max_single._count << " generation." << endl;
    }
};

void GA::Init() {
    int i, j, wsum, vsum;
    // 对于整个算法，产生PS个 **个体** 组成的 **种群**
    for (i = 0; i < PS; i++) {
        wsum = 0;
        vsum = 0;
        // 对每个个体进行
        for (j = 0; j < NUM; j++) {
            // 对每个基因随机编码 => 随机设定是否存放对应物品
            zq[i].gene[j] = pp();

            // 对于每个个体计算总的重量和体积，对个体的筛选是基于这些数据的
            wsum += zq[i].gene[j] * zl[j];
            vsum += zq[i].gene[j] * tj[j];
        }
        //对于随机生成的个体进行约束，保证产生符合条件的个体
        if (wsum > PACK_MAX_W || vsum > PACK_MAX_V)
            i--;  // 不满足要求，索引回退，重新产生
    }
}

// 计算第row个个体的总的价值
int GA::Cal_SingleValue(int row) {
    int j, valuesum = 0;
    for (j = 0; j < NUM; j++) {
        valuesum += zq[row].gene[j] * value[j];
    }
    zq[row].sum_val = valuesum;
    return valuesum;
}

// 计算第row个个体的总的重量
int GA::Cal_SingleW(int row) {
    int j, wsum = 0;
    for (j = 0; j < NUM; j++) {
        wsum += zq[row].gene[j] * zl[j];
    }
    zq[row].sum_w = wsum;
    return wsum;
}

// 计算第row个个体的总的体积
int GA::Cal_SingleV(int row) {
    int j, vsum = 0;
    for (j = 0; j < 32; j++) {
        vsum += zq[row].gene[j] * tj[j];
    }
    zq[row].sum_v = vsum;
    return vsum;
}

// 计算所有个体的 **适应度**
void GA::Cal_Fitness() {
    int i, w, v, val;
    // 遍历所有的个体，得到他们对应的三个指标
    // 不满足条件的适应度置为0
    // 满足条件的将适应度设置为对应的价值（最终要筛选价值确定最佳个体）
    for (i = 0; i < PS; i++) {
        w = Cal_SingleW(i);
        v = Cal_SingleV(i);
        val = Cal_SingleValue(i);
        if (w > PACK_MAX_W || v > PACK_MAX_V) {
            zq[i].fit = 0;
        } else {
            zq[i].fit = val;
        }
    }
}

// 计算最大价值个体，也就是适应度最高的个体
void GA::Cal_Maxval_Single(int _generation) {
    int maxval = zq[0].fit;
    int id = 0;

    // 对于所有的个体进行遍历，这里实际上是在寻找最大的适应度的个体
    for (int i = 0; i < PS; i++) {
        if (maxval < zq[i].fit) {
            maxval = zq[i].fit;
            id = i;
        }
    }

    // 纪录最大的适应度，同时记录当前遗传的代数
    if (maxval > max_single.fit) {
        max_single = zq[id];
        max_single._count = _generation;
    }
}

// 开始执行选择操作
void GA::Select() {
    int fit_sum = 0;
    int i, j;
    float rand_rate, cur_rate;
    float selected_rate[PS];
    Entity new_zq[PS];

    // 对所有的个体的适应度进行汇总
    for (i = 0; i < PS; i++) {
        fit_sum += zq[i].fit;
    }

    // 使用轮赌法进行选择

    // 这里计算各各个个体占总体的比例，selected_rate总存放的是递加式的比例值
    selected_rate[0] = float(zq[0].fit) / fit_sum;
    for (i = 1; i < PS; i++) {
        cur_rate = selected_rate[i - 1] + float(zq[i].fit) / fit_sum;
        selected_rate[i] = cur_rate;
    }

    for (i = 0; i < PS; i++) {
        // 为了构建新的种群，依旧是PS个个体，对于旧的种群需要进行筛选
        // 每个新的个体都来自每次都会将适应度之和小于一个随机值的所有个体抛弃，仅来自于刚好适应度和超过这个随机值的个体
        rand_rate = (rand() % 1000 / 1000.0);
        for (j = 0; j < PS; j++) {
            if (rand_rate <= selected_rate[j]) {
                new_zq[i] = zq[j];
                break;
            }
        }
    }

    // 更新旧的种群
    for (i = 0; i < PS; i++) {
        zq[i] = new_zq[i];
    }

}

// 个体之间两两进行交叉，每个个体只交叉一次
void GA::Cross() {
    int i, j;
    for (i = 0; i < PS - 1; i += 2) {
        Entity en1 = zq[i];
        Entity en2 = zq[i + 1];

        for (j = 0; j < NUM; j++) {
            // 根据生成的随机概率判定是否交叉第j个gene，也就是第j个物品
            if (IsCross()) {
                int tmp = en1.gene[j];
                en1.gene[j] = en2.gene[j];
                en2.gene[j] = tmp;
            }
        }
        zq[i] = en1;
        zq[i + 1] = en2;
    }
}

// 执行变异
void GA::Variation() {
    int i, j;
    for (i = 0; i < PS; i++) {
        // 根据随机概率判定第i个个体是否变异
        if (IsVariation()) {
            for (j = 0; j < NUM; j++) {
                if (IsVariation()) {
                    // 对于第j个物品的存在与否进行取反操作，这里以三目运算符的形式实现了对于0、1的取反，所谓变异
                    zq[i].gene[j] = zq[i].gene[j] ? 0 : 1;
                }
            }
        }
    }
}

int main() {
    srand(time(NULL));
    GA temp;
    temp.Run();
    return 0;
}
