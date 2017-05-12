#include<iostream>
#include<vector>
#include<fstream>
#include<ctime>
#include<cmath>
#define MAX 999

using namespace std;

vector<vector<int>> matrix;						//�ڽӾ���
vector<vector<int>> group;						//��ʼȺ��
vector<double> fitness;							//��Ӧ��
int node_count;									//�ڵ�����
int generation_num = 0;							//��������  
double mutation_rate = 0.3;						//������ 0.3�ó���ȷ����ĸ��ʽϸ�
double crossover_rate = 0.8;					//������


void Set(){

	ifstream infile;
	infile.open("data.txt");   //���ļ����������ļ��������� 

	//�ڵ�����
	infile >> node_count;
	matrix.resize(node_count);
	for (int i = 0; i < node_count; i++){
		matrix[i].resize(node_count);
	}

	//�����ڽӾ�����Ϣ
	int c;
	for (int i = 0; i < node_count; i++){
		for (int j = 0; j < node_count; j++){
			infile >> c;
			matrix[i][j] = c;
		}
	}
	infile.close();             //�ر��ļ������� 

}
void InitialPopulation(){
	srand((unsigned)time(NULL));
	int population_quantity = node_count * 2; //��ʼ��Ⱥ����
	group.resize(population_quantity);
	fitness.resize(population_quantity);

	for (int i = 0; i < population_quantity; i++){
			group[i].push_back(0);//��ʼ��
			int length = 1;       //group����
			while (group[i][length - 1] != node_count - 1){//��·����δѰ�ҵ��յ�
				//���ѡ��ڵ�
				int node = rand() % node_count;

				if (matrix[group[i][length - 1]][node] > 0 && matrix[group[i][length - 1]][node] < MAX){
					//�ڵ��ѡ
					double rate = rand() % 100 / 100.0;
					//����ڵ���С�����鵱ǰ���ڵ� ����ʲ�ѡ��
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
	//����·�����ܳ��ȣ������ͨ������0
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
	//��������
	vector<double> roulette;
	srand((unsigned)time(NULL));

	double temp = 0;
	for (int i = 0; i < group.size(); i++){
		roulette.push_back(temp + fitness[i]);
		temp += fitness[i];
	}
	//ѡ�����һ����ţ������select��

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

	//�ؽ�Ⱥ��
	vector<vector<int>> tempGroup(group);//����
	for (int i = 0; i < select.size(); i++){
		group[i].resize(tempGroup[select[i]].size());
		for (int j = 0; j < group[i].size(); j++)
			group[i][j] = tempGroup[select[i]][j];
	}

}
void PMX(){
	//����ƥ�佻��
	srand((unsigned)time(NULL));
	for (int i = 0; i < group.size() - 1; i += 2){
		//�жϽ�����ʣ��������������ѭ��
		if ((rand() % 100 / 100.0) > crossover_rate){
			continue;
		}
		int count = 0;  //�ظ�����

		while (count < 10){//�ظ�����Ҳ������ʽ���㣬����
			//���ѡ�����������
			int length = 0;
			group[i].size()>group[i + 1].size() ?
				length = group[i + 1].size() :
				length = group[i].size();
			int point_1 = rand() % (length - 2) + 1;
			int point_2 = rand() % (length - 2) + 1;

			//ʹ�õ�1�ȵ�2С
			if (point_1 == point_2)
				continue;
			if (point_1 > point_2){
				int temp = point_2;
				point_2 = point_1;
				point_1 = temp;
			}
			//������·���Ƿ�ɻ�
			vector<int> temp1(group[i]);
			vector<int> temp2(group[i + 1]);
			for (int j = point_1; j <= point_2; j++){
				int temp = temp1[j];
				temp1[j] = temp2[j];
				temp2[j] = temp;

			}
			//�жϽ���������·���Ƿ��ͨ,��ͨ���ҽ�����·�����̣���ѡ�񽻻�
			int d1 = Evaluate(temp1), d2 = Evaluate(temp2);
			if (d1 && d2 && (d1<Evaluate(group[i]) || d2<Evaluate(group[i + 1]))){
				group[i].swap(temp1);
				group[i + 1].swap(temp2);
				break;//����ѭ��
			}
			else
				count++;


		}

	}
}
void IX(){
	//����ʽ����
	srand((unsigned)time(NULL));
	for (int i = 0; i < group.size() - 1; i += 2){
		//�жϽ�����ʣ�����0.8���������ѭ��
		if ((rand() % 100 / 100.0) > crossover_rate)
			continue;


	}
}
void Cross(){//����

	PMX();
}
void Mutation(){

	srand((unsigned)time(NULL));
	for (int i = 0; i < group.size(); i++){
		double rate = rand() % 100 / 100.0;
		//����
		if (rate < mutation_rate){
			rate = rand() % 100 / 100.0;
			//ѡ�����
			int try_times = 0;
			if (rate > 0.5){
				while (try_times < group[i].size()){
					vector<int> temp(group[i]);
					int start = rand() % (group[i].size() - 2) + 1; //����յ��������
					int node = rand() % node_count;//����Ľڵ�
					temp.insert(temp.begin() + start + 1, node);//����
					int d = Evaluate(temp);
					//�����·����ͨ���ұ�ԭ·����
					if (d != 0 && d < Evaluate(group[i])){
						group[i].insert(group[i].begin() + start + 1, node);//����
						break;
					}

					try_times++;
				}
			}
			//ѡ��ɾ��
			else{
				while (try_times < group[i].size()){
					vector<int> temp(group[i]);
					int start = rand() % (group[i].size() - 2) + 1; //����յ��������
					temp.erase(temp.begin() + start);//ɾ��
					int d = Evaluate(temp);
					//�����·����ͨ���ұ�ԭ·����
					if (d != 0 && d < Evaluate(group[i])){
						group[i].erase(group[i].begin() + start);//ɾ��
						break;
					}

					try_times++;
				}
			}

		}
	}

}
void Genetic(){
	Select();//ѡ��
	Cross();//����
	Mutation();//����
}
void Generation(){

	for (int i = 0; i < 100; i++){
		cout << i << endl;
		//���������Ӧ��
		for (int i = 0; i < group.size(); i++){
			int distance = Evaluate(group[i]);
			fitness[i] = 1.0 / distance;
		}
		//�Ŵ�
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

	//�����ڽӾ���
	Set();
	//������ʼȺ��
	InitialPopulation();
	//����
	Generation();
system("pause");
}