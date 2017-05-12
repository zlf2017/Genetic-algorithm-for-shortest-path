#include<iostream>
#include<vector>
#include<fstream>
#include<ctime>
#include<cmath>
#define MAX 999

using namespace std;

vector<vector<int>> matrix;						//邻接矩阵
vector<vector<int>> group;						//初始群体
vector<double> fitness;							//适应度
int node_count;									//节点总数
int generation_num = 0;							//进化代数  
double mutation_rate = 0.3;						//变异率 0.3得出正确结果的概率较高
double crossover_rate = 0.8;					//交叉率


void Set(){

	ifstream infile;
	infile.open("data.txt");   //将文件流对象与文件连接起来 

	//节点总数
	infile >> node_count;
	matrix.resize(node_count);
	for (int i = 0; i < node_count; i++){
		matrix[i].resize(node_count);
	}

	//输入邻接矩阵信息
	int c;
	for (int i = 0; i < node_count; i++){
		for (int j = 0; j < node_count; j++){
			infile >> c;
			matrix[i][j] = c;
		}
	}
	infile.close();             //关闭文件输入流 

}
void InitialPopulation(){
	srand((unsigned)time(NULL));
	int population_quantity = node_count * 2; //初始种群数量
	group.resize(population_quantity);
	fitness.resize(population_quantity);

	for (int i = 0; i < population_quantity; i++){
			group[i].push_back(0);//起始点
			int length = 1;       //group长度
			while (group[i][length - 1] != node_count - 1){//当路径尚未寻找到终点
				//随机选择节点
				int node = rand() % node_count;

				if (matrix[group[i][length - 1]][node] > 0 && matrix[group[i][length - 1]][node] < MAX){
					//节点可选
					double rate = rand() % 100 / 100.0;
					//如果节点标记小于数组当前最后节点 大概率不选择
					if (node<group[i][length - 1] && rate>0.8){
							group[i].push_back(node);
							length++;
					}
					else if (node>group[i][length - 1]){
						group[i].push_back(node);
						length++;
					}
				}
				

			}

	}
}
int Evaluate(vector<int> &sample){
	//返回路径的总长度，如果不通，返回0
	int distance = 0;
	for (int j = 0; j < sample.size() - 1; j++){
		int k = sample[j], l = sample[j + 1];
		if (matrix[k][l] == MAX)
			return 0;
		if (matrix[k][l] == 0){
			sample.erase(sample.begin() + j);
			j--;
		}

		distance += matrix[k][l];
	}
	return distance;


}
void Select(){
	//创建轮盘
	vector<double> roulette;
	srand((unsigned)time(NULL));

	double temp = 0;
	for (int i = 0; i < group.size(); i++){
		roulette.push_back(temp + fitness[i]);
		temp += fitness[i];
	}
	//选择的下一代编号，存放在select中

	vector<int> select;
	double total = 0;
	for (int i = 0; i < ::fitness.size(); i++)
		total += fitness[i];

	for (int j = 0; j < group.size(); j++){
		double num = rand() % (int)(total * 100) / 100.0;
		for (int i = 0; i < roulette.size(); i++){
			if (num>roulette[i])
				continue;
			select.push_back(i);
			break;
		}
	}

	//重建群体
	vector<vector<int>> tempGroup(group);//副本
	for (int i = 0; i < select.size(); i++){
		group[i].resize(tempGroup[select[i]].size());
		for (int j = 0; j < group[i].size(); j++)
			group[i][j] = tempGroup[select[i]][j];
	}

}
void PMX(){
	//部分匹配交叉
	srand((unsigned)time(NULL));
	for (int i = 0; i < group.size() - 1; i += 2){
		//判断交叉概率，大于则跳过这次循环
		if ((rand() % 100 / 100.0) > crossover_rate){
			continue;
		}
		int count = 0;  //重复次数

		while (count < 10){//重复多次找不到合适交叉点，放弃
			//随机选择两个交叉点
			int length = 0;
			group[i].size()>group[i + 1].size() ?
				length = group[i + 1].size() :
				length = group[i].size();
			int point_1 = rand() % (length - 2) + 1;
			int point_2 = rand() % (length - 2) + 1;

			//使得点1比点2小
			if (point_1 == point_2)
				continue;
			if (point_1 > point_2){
				int temp = point_2;
				point_2 = point_1;
				point_1 = temp;
			}
			//交叉点的路径是否可换
			vector<int> temp1(group[i]);
			vector<int> temp2(group[i + 1]);
			for (int j = point_1; j <= point_2; j++){
				int temp = temp1[j];
				temp1[j] = temp2[j];
				temp2[j] = temp;

			}
			//判断交叉后的两条路径是否可通,可通并且交换后路径更短，才选择交换
			int d1 = Evaluate(temp1), d2 = Evaluate(temp2);
			if (d1 && d2 && (d1<Evaluate(group[i]) || d2<Evaluate(group[i + 1]))){
				group[i].swap(temp1);
				group[i + 1].swap(temp2);
				break;//跳出循环
			}
			else
				count++;


		}

	}
}
void IX(){
	//插入式交叉
	srand((unsigned)time(NULL));
	for (int i = 0; i < group.size() - 1; i += 2){
		//判断交叉概率，大于0.8则跳过这次循环
		if ((rand() % 100 / 100.0) > crossover_rate)
			continue;


	}
}
void Cross(){//交叉

	PMX();
}
void Mutation(){

	srand((unsigned)time(NULL));
	for (int i = 0; i < group.size(); i++){
		double rate = rand() % 100 / 100.0;
		//变异
		if (rate < mutation_rate){
			rate = rand() % 100 / 100.0;
			//选择插入
			int try_times = 0;
			if (rate > 0.5){
				while (try_times < group[i].size()){
					vector<int> temp(group[i]);
					int start = rand() % (group[i].size() - 2) + 1; //起点终点无需变异
					int node = rand() % node_count;//插入的节点
					temp.insert(temp.begin() + start + 1, node);//插入
					int d = Evaluate(temp);
					//如果新路径可通并且比原路径短
					if (d != 0 && d < Evaluate(group[i])){
						group[i].insert(group[i].begin() + start + 1, node);//插入
						break;
					}

					try_times++;
				}
			}
			//选择删除
			else{
				while (try_times < group[i].size()){
					vector<int> temp(group[i]);
					int start = rand() % (group[i].size() - 2) + 1; //起点终点无需变异
					temp.erase(temp.begin() + start);//删除
					int d = Evaluate(temp);
					//如果新路径可通并且比原路径短
					if (d != 0 && d < Evaluate(group[i])){
						group[i].erase(group[i].begin() + start);//删除
						break;
					}

					try_times++;
				}
			}

		}
	}

}
void Genetic(){
	Select();//选择
	Cross();//交叉
	Mutation();//变异
}
void Generation(){

	for (int i = 0; i < 100; i++){
		cout << i << endl;
		//计算个体适应度
		for (int i = 0; i < group.size(); i++){
			int distance = Evaluate(group[i]);
			fitness[i] = 1.0 / distance;
		}
		//遗传
		Genetic();
	}

	double min_distance = MAX;
	int id = -1;
	for (int i = 0; i < fitness.size(); i++){
		if (min_distance > 1 / fitness[i]){
			min_distance = 1 / fitness[i];
			id = i;
		}
			
		
	}

	cout << "Shortest path length is: " << min_distance << endl;
	cout << "Shortest path is:";
	for (int j = 0; j < group[id].size(); j++)
			cout << "  " << group[id][j];

}



void main(){

	//建立邻接矩阵
	Set();
	//建立初始群体
	InitialPopulation();
	//进化
	Generation();
system("pause");
}