#include <iostream>
#include "sgpri_mod3.cpp"
#include <random>
#include <omp.h>
#include <set>
#include "mpi.h"

using namespace sgpri_m3;

//bool compareSetStr(std::set<std::string> a, std::set<std::string> b)
//{
//	return (a == b);
//}
std::set<std::string> Phi_AminusB_forOpenMP_allstates_BfAver(
	std::string la, std::set<std::string> lb, std::set<std::string> u,
	std::set<std::string> opref_B)
{
	std::set<std::string> D_res;
	std::set<std::string> D_help;
	for (auto it = u.begin(); it != u.end(); it++)
	{
		D_help.insert((*it) + la);
	}
	int flag = 1;
	while (flag)
	{
		if (D_help.size() == 0) break;
		std::string v = *(D_help.begin());
		D_help.erase(v);
		for (auto it = lb.begin(); it != lb.end(); it++)
		{
			std::string tmp_u = *(it);
			if (v.substr(0, tmp_u.length()) == tmp_u)
			{
				D_help.insert(v.substr(tmp_u.length(), v.length()));
			}
		}
		if (opref_B.find(v) != opref_B.end())
		{
			D_res.insert(v);
		}
	}
	return D_res;
}
int findLanguageInSubLangs(std::set<std::string> l,
	std::vector<std::set<std::string>> subl,
	std::vector<std::pair<int, std::vector<int*>>> m)
{
	int cnt = l.size(), b = 0;
	//if (cnt == 0 && subl[0].size() == 0) return 0;
	int i = 0;
	for (i = 0; i < m.size(); i++)
	{
		if (m[i].first != cnt) b += m[i].second.size();
		else
		{
			cnt = b + m[i].second.size() + 1;
			break;
		}
	}
	for (; b < cnt; b++)
	{
		if (l == subl[b])
			return b;
	}
	return -1;
}


int main11()
{//openmp version, all states
#if _OPENMP
	omp_set_num_threads(16);
	auto begin = omp_get_wtime();
#pragma omp parallel
	{
		int idx = omp_get_thread_num(), 
			thn = omp_get_num_threads();
		int cnt = 0;
		Language* a = readLangFromString("{a,ab,abba}");
		Language* b = readLangFromString("{a,aab,baababab}");
		std::set<std::string> bb = b->getSetStr_withClean();
		auto b_asl = getAllSublang(choosePrefSuffLang(b, "BfollowA"));
		std::vector<std::set<std::string>> qb;
		for (auto& e : b_asl.second) 
			qb.push_back(e->getSetStr_withClean());
		std::vector<std::pair<int, std::vector<int*>>> ssm = b_asl.first;
		std::set<std::string> oprefB = choosePrefSuffLang(
			b, "BfollowA")->getSetStr_withClean();
		std::vector<Language*> AbyWord;
		std::vector<std::string> awords;
		for (auto& e : a->getWords())
		{
			awords.push_back(e.first);
			Language* tmpl = new Language(a->getAlphabet());
			tmpl->addWord_(e.first);
			tmpl->cleanBadWords();
			AbyWord.push_back(tmpl);
		}
		int qbsize = qb.size(), awsize = awords.size();
		int* tablePRI = new int[qbsize * awsize];
		int i = 0, j = 0, k = 0;
		for (i = idx; i < qbsize * awsize; i += thn) tablePRI[i] = -1;
		for (i = idx; i < qbsize * awsize; i += thn)
		{
			tablePRI[i] = findLanguageInSubLangs(
				Phi_AminusB_forOpenMP_allstates_BfAver(
					awords[i % awsize], bb, qb[i / awsize], oprefB),
				qb, ssm);
			cnt++;
		}
		std::cout << "no." << idx << "counted:" << cnt << std::endl;
	}
	auto finish = omp_get_wtime();
	std::cout << "time: " << finish - begin << std::endl;
#endif
	return 0;
}
int main12(int argc, char* argv[])
{//mpi version, all states
	int mpirank, mpisize;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpirank);
	MPI_Comm_size(MPI_COMM_WORLD, &mpisize);
	auto begin = MPI_Wtime();
	//
	int cnt = 0;
	Language* a = readLangFromString("{a,ab,abba}");
	Language* b = readLangFromString("{a,aab,baababab}");
	std::set<std::string> bb = b->getSetStr_withClean();
	auto b_asl = getAllSublang(choosePrefSuffLang(b, "BfollowA"));
	std::vector<std::set<std::string>> qb;
	for (auto& e : b_asl.second) 
		qb.push_back(e->getSetStr_withClean());
	std::vector<std::pair<int, std::vector<int*>>> ssm = b_asl.first;
	std::set<std::string> oprefB = choosePrefSuffLang(
		b, "BfollowA")->getSetStr_withClean();
	std::vector<Language*> AbyWord;
	std::vector<std::string> awords;
	for (auto& e : a->getWords())
	{
		awords.push_back(e.first);
		Language* tmpl = new Language(a->getAlphabet());
		tmpl->addWord_(e.first);
		tmpl->cleanBadWords();
		AbyWord.push_back(tmpl);
	}
	int qbsize = qb.size(), awsize = awords.size();
	int* tablePRI = new int[qbsize * awsize];
	int i = 0, j = 0, k = 0;
	if (mpirank == 0) 
		for (i = 0; i < qbsize; i++) 
			for (j = 0; j < awsize; j++) tablePRI[i * awsize + j] = -1;
	for (i = mpirank; i < qbsize * awsize; i += mpisize)
	{
		tablePRI[i] = findLanguageInSubLangs(
			Phi_AminusB_forOpenMP_allstates_BfAver(
				awords[i % awsize], bb, qb[i / awsize], oprefB),
			qb, ssm);
		cnt++;
	}
	//
	MPI_Request mpirq;
	if (mpirank)
	{
		tablePRI[0] = mpirank;
		MPI_Isend(tablePRI, qbsize * awsize, 
			MPI_INT, 0, 1, MPI_COMM_WORLD, &mpirq);
	}
	else
	{
		i = mpisize - 1;
		while (i)
		{
			int* tmprecv = new int[qbsize * awsize];
			tmprecv[0] = -1;
			MPI_Irecv(tmprecv, qbsize * awsize, MPI_INT, 
				MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &mpirq);
			MPI_Wait(&mpirq, MPI_STATUS_IGNORE);
			if (tmprecv[0] > 0)
			{
				for (k = tmprecv[0]; k < qbsize * awsize; k += mpisize) 
					tablePRI[k] = tmprecv[k];
				i--;
			}
			delete[] tmprecv;
		}
	}
	//
	auto finish = MPI_Wtime();
	std::cout << "no." << mpirank << " counted:" << cnt
		<< " time: " << finish - begin << std::endl;
	MPI_Finalize();
	return 0;
}
int main14(int argc, char* argv[])
{//mpi version, attainable, works
	MPI_Init(&argc, &argv);
	int mpirank, mpisize;
	int newlangamount = 0;
	int cnt = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &mpirank);
	MPI_Comm_size(MPI_COMM_WORLD, &mpisize);
	auto begin = MPI_Wtime();
	//
	Language* a = readLangFromString("{a,ab,abba}");
	Language* b = readLangFromString(
		"{a,aab,baabababab,bababaababa}");
	auto b_asl = getAllSublang(choosePrefSuffLang(
		b, "BfollowA"));
	std::vector<Language*> AbyWord;
	for (auto& e : a->getWords())
	{
		Language* tmpl = new Language(a->getAlphabet());
		tmpl->addWord_(e.first);
		tmpl->cleanBadWords();
		AbyWord.push_back(tmpl);
	}
	int qbsize = b_asl.second.size(), awsize = AbyWord.size();
	int i = 0, j = 0;
	//
	if (mpirank == 0)
	{
		int* transformindex = new int[qbsize];
		std::vector<Language*> att;
		for (i = 2; i < qbsize; i++) transformindex[i] = -1;
		transformindex[0] = 0;
		transformindex[1] = 1;
		int round = 2;
		std::vector<int*> tablePRI_vec;
		std::set<int> newlang, attained;
		attained.insert(0);
		attained.insert(1);
		att.push_back(b_asl.second[0]);
		att.push_back(b_asl.second[1]);
		for (i = 0; i < 2; i++)
		{
			int* tmpline = new int[awsize];
			for (j = 0; j < awsize; j++)
			{
				tmpline[j] = findLanguageInSubLangs(
					Phi_AminusB(
						AbyWord[j], b, b_asl.second[i], "BfollowA"),
					b_asl);
				if (attained.find(tmpline[j]) == attained.end()) 
					newlang.insert(tmpline[j]);
				cnt++;
			}
			tablePRI_vec.push_back(tmpline);
		}
		newlangamount = newlang.size();
		MPI_Bcast(&newlangamount, 1, 
			MPI_INT, 0, MPI_COMM_WORLD);
		while (newlangamount)
		{
			int* newlangpool = new int[newlangamount]();
			i = 0;
			for (int e : newlang)
			{
				newlangpool[i] = e;
				attained.insert(e);
				i++;
				transformindex[e] = round;
				round++;
				att.push_back(b_asl.second[e]);
			}
			newlang.clear();
			MPI_Bcast(newlangpool, newlangamount, 
				MPI_INT, 0, MPI_COMM_WORLD);
			int* reslangpool = new int[newlangamount * awsize];
			reslangpool[0] = -1;
			for (i = mpirank; i < newlangamount * awsize; i += mpisize)
			{
				reslangpool[i] = findLanguageInSubLangs(
					Phi_AminusB(AbyWord[i % awsize], b, 
						b_asl.second[newlangpool[i / awsize]], "BfollowA"),
					b_asl);
				if (attained.find(reslangpool[i]) == attained.end()) 
					newlang.insert(reslangpool[i]);
				cnt++;
			}
			j = mpisize - 1;
			while (j)
			{
				int* reslangrecv = new int[newlangamount * awsize];
				MPI_Request mpirq;
				MPI_Irecv(reslangrecv, newlangamount * awsize, MPI_INT,
					MPI_ANY_SOURCE, MPI_ANY_TAG, 
					MPI_COMM_WORLD, &mpirq);
				MPI_Wait(&mpirq, MPI_STATUS_IGNORE);
				for (i = reslangrecv[0]; i < newlangamount * awsize; i += mpisize)
				{
					reslangpool[i] = reslangrecv[i];
					if (attained.find(reslangpool[i]) == attained.end()) 
						newlang.insert(reslangpool[i]);
				}
				j--;
			}
			for (i = 0; i < newlangamount; i++)
			{
				int* tmpline = new int[awsize];
				for (j = 0; j < awsize; j++)
					tmpline[j] = reslangpool[i * awsize + j];
				tablePRI_vec.push_back(tmpline);
			}
			newlangamount = newlang.size();
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Bcast(&newlangamount, 1, 
				MPI_INT, 0, MPI_COMM_WORLD);
		}
		//
		int* tablePRI = new int[attained.size() * awsize];
		for (i = 0; i < attained.size(); i++)
		{
			for (j = 0; j < awsize; j++)
			{
				tablePRI[i * awsize + j] = 
					transformindex[tablePRI_vec[i][j]];
			}
		}
		while (tablePRI_vec.size())
		{
			auto it = tablePRI_vec.begin();
			delete[](*it);
			tablePRI_vec.erase(it);
		}
		auto finish = MPI_Wtime();
		std::cout << "no." << mpirank << " counted:" << cnt
			<< " time: " << finish - begin << std::endl;
	}
	else
	{
		newlangamount = 0;
		MPI_Bcast(&newlangamount, 1, MPI_INT, 0, MPI_COMM_WORLD);
		while (newlangamount > 0)
		{
			int* newlangpool = new int[newlangamount];
			MPI_Bcast(newlangpool, newlangamount, 
				MPI_INT, 0, MPI_COMM_WORLD);
			int* reslangpool = new int[newlangamount * awsize];
			reslangpool[0] = -1;
			for (i = mpirank; i < newlangamount * awsize; i += mpisize)
			{
				reslangpool[i] = findLanguageInSubLangs(
					Phi_AminusB(AbyWord[i % awsize], b, 
						b_asl.second[newlangpool[i / awsize]], "BfollowA"),
					b_asl);
				cnt++;
			}
			reslangpool[0] = mpirank;
			MPI_Request mpirq;
			MPI_Isend(reslangpool, newlangamount * awsize, 
				MPI_INT, 0, mpirank, MPI_COMM_WORLD, &mpirq);
			MPI_Wait(&mpirq, MPI_STATUS_IGNORE);
			delete[] newlangpool;
			delete[] reslangpool;
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Bcast(&newlangamount, 1, 
				MPI_INT, 0, MPI_COMM_WORLD);
		}
		auto finish = MPI_Wtime();
		std::cout << "no." << mpirank << " counted:" << cnt
			<< " time: " << finish - begin << std::endl;
	}
	//
	MPI_Finalize();
	return 0;
}
typePRI_1 tablePRI_groupoid_mpi(
	Language* bl, std::string option,
	int mpiRank, int mpiSize, int mpiMain, MPI_Comm mpiComm, 
	bool timer = true)
{
	auto begin = MPI_Wtime();
	int cnt = 0;
	auto sublan_pair = getAllSublang(
		choosePrefSuffLang(bl, option));
	std::vector<Language*> sublanBL = sublan_pair.second;
	std::vector<std::pair<
		int, std::vector<int*>>> subsetmodel = sublan_pair.first;
	int tableSize = sublanBL.size();
	int* tableSG = new int[tableSize * tableSize]();
	for (int i = mpiRank; i < tableSize * tableSize; i += mpiSize)
	{
		tableSG[i] = -1;
		Language* resCov = operationSemigroup_F(bl, 
			sublanBL[(int)(i / tableSize)], sublanBL[(int)(i % tableSize)], 
			option);
		resCov->cleanBadWords(false);
		tableSG[i] = findLanguageInSubLangs(
			resCov, sublanBL, subsetmodel);
		delete resCov;
		cnt++;
	}
	if (mpiRank != mpiMain)
	{
		tableSG[0] = mpiRank;
		MPI_Request req;
		MPI_Isend(tableSG, tableSize * tableSize, 
			MPI_INT, mpiMain, 0, mpiComm, &req);
		MPI_Wait(&req, MPI_STATUS_IGNORE);
		for (int i = 0; i < subsetmodel.size(); i++)
			for (int j = 0; j < subsetmodel[i].second.size(); j++)
				delete[] subsetmodel[i].second[j];
		std::vector<std::pair<
			int, std::vector<int*>>>().swap(subsetmodel);
	}
	else
	{
		int waitNum = mpiSize;
		while (waitNum > 1)
		{
			int* tabletemp = new int[tableSize * tableSize]();
			MPI_Request req;
			MPI_Irecv(tabletemp, tableSize * tableSize, MPI_INT,
				MPI_ANY_SOURCE, MPI_ANY_TAG, mpiComm, &req);
			MPI_Wait(&req, MPI_STATUS_IGNORE);
			for (int i = tabletemp[0]; i < tableSize * tableSize; i += mpiSize)
				tableSG[i] = tabletemp[i];
			delete[] tabletemp;
			waitNum--;
		}
		tableSG[0] = 0;
	}
	if (timer)
	{
		auto finish = MPI_Wtime();
		std::cout << "No." << mpiRank << " counted:" << cnt
			<< " time: " << finish - begin << std::endl;
	}
	return std::make_pair(std::make_pair(bl, sublanBL), 
		std::make_pair(tableSize, tableSG));
}
int main212(int argc, char* argv[])
{//mpi version, groupoid table, no asserting
	MPI_Init(&argc, &argv);
	int mpirank, mpisize;
	MPI_Comm_rank(MPI_COMM_WORLD, &mpirank);
	MPI_Comm_size(MPI_COMM_WORLD, &mpisize);
	auto testlan = readLangFromString("{aabbabbba}");
	auto res = tablePRI_groupoid_mpi(testlan, "BfollowA",
		mpirank, mpisize, 0, MPI_COMM_WORLD, true);
	MPI_Finalize();
	return 0;
}
std::pair<std::vector<int*>, bool*>  binaryRelation_table_mpi(
	pri1_table p, int mpiRank, int mpiSize, int mpiMain, 
	MPI_Comm mpiComm, bool timer = true)
{
	std::vector<int*> cmbnNModel = myCmbntnN_noRe(p.first, 2);
	int i = 0, j = 0, cmbnSize = cmbnNModel.size(), cnt = 0;
	bool* table = new bool[cmbnSize * cmbnSize];
	auto begin = MPI_Wtime();
	for (i = mpiRank; i < cmbnSize * cmbnSize; i += mpiSize)
	{
		table[i] = binaryRelation_22s(p,
			cmbnNModel[(int)(i / cmbnSize)][0], 
			cmbnNModel[(int)(i / cmbnSize)][1],
			cmbnNModel[(int)(i % cmbnSize)][0], 
			cmbnNModel[(int)(i % cmbnSize)][1]);
		cnt++;
	}
	if (mpiRank != mpiMain)
	{
		table[mpiMain] = mpiRank;
		MPI_Request req;
		MPI_Isend(table, cmbnSize * cmbnSize, MPI_BYTE, 
			mpiMain, 0, mpiComm, &req);
		MPI_Wait(&req, MPI_STATUS_IGNORE);
		std::vector<int*>().swap(cmbnNModel);
		delete[] table;
		table = nullptr;
	}
	else
	{
		int waitNum = mpiSize;
		while (waitNum > 1)
		{
			bool* tabletemp = new bool[cmbnSize * cmbnSize];
			MPI_Request req;
			MPI_Irecv(tabletemp, cmbnSize * cmbnSize, MPI_BYTE,
				MPI_ANY_SOURCE, MPI_ANY_TAG, mpiComm, &req);
			MPI_Wait(&req, MPI_STATUS_IGNORE);
			for (int i = tabletemp[mpiMain]; i < cmbnSize * cmbnSize; i += mpiSize)
				table[i] = tabletemp[i];
			delete[] tabletemp;
			waitNum--;
		}
	}
	if (timer)
	{
		auto finish = MPI_Wtime();
		std::cout << "No." << mpiRank << " counted:" << cnt
			<< " time: " << finish - begin << std::endl;
	}
	return std::make_pair(cmbnNModel, table);
}
int main22(int argc, char* argv[])
{//mpi version, groupoid table & binary relation, no asserting
	MPI_Init(&argc, &argv);
	int mpirank, mpisize;
	MPI_Comm_rank(MPI_COMM_WORLD, &mpirank);
	MPI_Comm_size(MPI_COMM_WORLD, &mpisize);
	auto testlan = readLangFromString("{aaaaaaa}");
	auto res = tablePRI_groupoid_mpi(testlan, "BfollowA",
		mpirank, mpisize, 0, MPI_COMM_WORLD, true);
	MPI_Bcast(res.second.second, res.second.first * res.second.first, 
		MPI_INT, 0, MPI_COMM_WORLD);
	auto resbr = binaryRelation_table_mpi(
		res.second, mpirank, mpisize, 0, MPI_COMM_WORLD, true);
	MPI_Finalize();
	return 0;
}
int main(int argc, char* argv[])
{
	//main12(argc, argv);
	//main14(argc, argv);
	/*auto begin = omp_get_wtime();
	auto test = getTablePRI_twoLang(readLangFromString("{a,ab,abba}"),
		readLangFromString("{a,aab,baabababab,bababaababa}"), "BfollowA", true);
	std::cout << omp_get_wtime() - begin << std::endl;
	getTablePRI_twoLang_printProgram(test, 's');*/
	//main212(argc, argv);
	/*omp_set_num_threads(16);
	auto begin = omp_get_wtime();
	binaryRelation_table_omp(
		tablePRI_groupoid(readLangFromString("{aaaaaaa}"), "BfollowA").second,
		false);
	std::cout << "TIME: " << omp_get_wtime() - begin << std::endl;*/
	main22(argc, argv);
	return 0;
}