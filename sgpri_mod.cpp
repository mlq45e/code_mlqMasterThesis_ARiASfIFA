#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <utility>
#include <fstream>
#include <time.h>
#include <cmath>
#include <windows.h>
#include <iomanip>

namespace sgpri_m3
{

	class Alphabet
	{
	protected:
		std::map<char, bool> sigma; // where save the alphabet itself
		int length; // how many letters are in the alphabet
	public:
		// there are 3 constructors: from nothing, from a string, from many strings
		Alphabet() 
		{ 
			length = 0; 
		}
		Alphabet(std::string s) 
		{
			length = 0;
			for (auto e : s)
			{
				if (sigma[e] == true) continue;
				else sigma[e] = true; length++;
			} 
		}
		Alphabet(std::vector<std::string> v) 
		{
			length = 0;
			for (std::string e : v)
			{
				for (auto ee : e)
				{
					if (sigma[ee] == true) continue;
					else sigma[ee] = true; length++;
				}
			} 
		}
		// destructor: 
		~Alphabet() 
		{
			sigma.clear(); 
		}
		int addLetter(char c) 
		{
			// add a letter if there wasn't it in alphabet
			// return how many letters are added
			if (sigma.find(c) != sigma.end()) return 0;
			else
			{
				sigma[c] = true;
				length++;
				return 1;
			}
			return -1; /* just in case */ 
		}
		bool askWordInSigmaStar(std::string s) 
		{
			// find out whether this word has only letters in this alphabet
			if (s.length() == 0) return true;
			for (auto e : s)
			{
				if (sigma.find(e) == sigma.end()) return false;
			}
			return true; 
		}
		std::string toStringSigma() 
		{
			// return a string to represent this alphabet which has many additional parts
			std::string s = "sigma[" + std::to_string(length) + "]{ ";
			for (auto& e : sigma)
			{
				s += e.first;
				s += ", ";
			}
			return ((s[s.length() - 2] == ',') ? (s.substr(0, s.length() - 2) + " }") : (s + "}")); 
		}
		std::string toStringLetters()
		{// return a string which has and only has letters of alphabet
			//means if provide it to constructor, will get an alphabet with the same letters
			std::string s = "";
			for (auto& e : sigma)
			{
				s += e.first;
			}
			return s;
		}
		int getLength() 
		{ 
			return length; 
		}
		int addLetters(std::string s) //return added letters amount
		{// add letters from a string which are not in the alphabet
			int flag = 0;
			if (s.length() == 0) return flag;
			for (char c : s)
			{
				if (sigma.find(c) == sigma.end())
				{
					sigma[c] = true;
					length++;
					flag++;
				}
			}
			return flag;
		}
		std::map<char, bool> getSigma() 
		{ 
			return sigma; 
		}
		bool compareAlphabet(Alphabet* abc)
		{// to compare alphabets
			return std::equal(this->sigma.begin(), this->sigma.end(), abc->getSigma().begin());
		}
	};


	class Language
	{
		int count; // amount of words
		Alphabet* ABC; // its alphabet
		std::map<std::string, int> words; // words of language with index
	public:
		// 4 constructors: from nothing, 
		// alphabet, 
		// word-member of another language and automatically make alphabet, 
		// another language
		Language() { count = 0; ABC = new Alphabet(); }
		Language(Alphabet* a) { count = 0; ABC = new Alphabet(a->toStringLetters()); }
		Language(std::map<std::string, int> stringmap)
		{
			ABC = new Alphabet();
			count = 0;
			for (auto& e : stringmap)
			{
				if (e.second > 0)
				{
					count++;
					words[e.first] = count;
					ABC->addLetters(e.first);
				}
			}
		}
		Language(Language* l)
		{
			ABC = new Alphabet(l->getAlphabet()->toStringLetters());
			count = 0;
			for (auto& e : l->getWords())
			{
				if (e.second > 0)
				{
					count++;
					words[e.first] = count;
				}
			}
		}
		~Language() { delete ABC; words.clear(); }
		int cleanBadWords(bool countCheck = false)//suggest: (false)
		{// since container map of STL C++ works strangely, need this method
			// here instruction for countCheck: 
			// if add words to a newly created language, false
			// if add words to an old language, better check it
			auto it = words.begin();
			while (it != words.end())
			{
				if (it->second <= 0 || !ABC->askWordInSigmaStar(it->first))
					it = words.erase(it);
				else
					it++;
			}
			int checker = count;
			count = 0;
			for (auto& e : words) { count++; e.second = count; }
			if (countCheck) //check==true : return oldcount-realcount
			{ return (checker - count); }
			else // check==false
			{ return 0; }
			return -1;
		}
		int addWord_(std::string s) 
		{// a fast method to add word without careful checking
			//may not add; return added word amount
			// may make index chaos
			// suggest: once finish all addWord, then cleanBadWord, then other works
			// this method doesn't consider alphabet, 
			// so after cleanBadWord maybe word won't be added in the end
			// ABC will never be changed
			if (words.find(s) != words.end()) return 0;
			else { count++; words[s] = count; return 1; }
			return -1;
		}
		int addWord(std::string s, bool alphabetCheck) // return added word amount
		{// add word with good checking
			// with index fixing part
			// if not check alphabet, ABC can be changed
			if (!ABC->askWordInSigmaStar(s))
			{
				if (alphabetCheck) return 0;
				else // if not check alphabet, ABC can be changed
					ABC->addLetters(s);
			}
			if (words.find(s) != words.end()) return 0;
			else { count++; words[s] = count; this->cleanBadWords(); return 1; }
			return -1;
		}
		int getCount() { return count; }
		bool askWordInLang(std::string s) { return (words.find(s) != words.end()); }
		std::string toStringLanguage() // suggest
		{// return a string for representing language with many additional parts
			std::string s = "Language[" + std::to_string(count) + "]{ ";
			for (auto& e : words) s += "<" + e.first + ":" + std::to_string(e.second) + "> "; 
			return (s + "}");
		}
		std::string represent_epsilon = ""; // it is only used for form string representation
		std::string toStringLang(std::string epsilon = "") 
		{// return a string for representing language with shorter additional parts
			std::string s = "{";
			for (auto& e : words) s +=
				((e.first.length()) ? e.first : ((epsilon == "") ? represent_epsilon : epsilon)) + ",";
			if (s.length() > 1) s.pop_back();
			return s + "}";
		}
		std::map<std::string, int> getWords() { return words; }
		std::vector<std::string> getVecStr_withClean()
		{
			this->cleanBadWords(false);
			std::vector<std::string> res;
			for (auto& e : words) res.push_back(e.first);
			return res;
		}
		Alphabet* getAlphabet() { return ABC; }
		std::string getWord_afterCleaning(int posit_index = 1)
		{// attention!: this counts "relative" index; use this!
			//default value means: to get the first word, use 1
			//index: from 1 to count
			//suggested usage: once cleanBadWord (after addWord), then always use this
			//after addWord "absolute index" may be chaos, so 'd better always use this
			if (posit_index > count) return "*BAD_POS_NO_WORD";
			auto it = words.begin();
			advance(it, posit_index - 1);
			return it->first;
		}
		int deleteWordByIndex(int index = 1) // "individual" index
		{
			// delete word, which has index
			// so maybe delete nothing, if exist bad words or
			// index > count (then return 0)
			// and maybe delete more than 1, if many words have the same index
			// default index = 1 means: use 0 never delete anything
			int flag = 0;
			for (auto it = words.begin(); it != words.end(); it++)
				if (it->second == index) { it = words.erase(it); flag++; break; } 
			this->cleanBadWords(false); // good
			return flag;
		}
		int deleteWordByPosition(int pos = 1) // "relative" position; pos > 0
		{
			// bad words won't disturb
			// not as individual index, when pos > count, return -1
			// otherwise, always delete somthing
			// default pos = 1 means: to delete the 1st word, use 1
			if ((pos < 0) || (pos > words.size())) return -1;
			if (pos == 0) return 0;
			auto it = words.begin();
			advance(it, pos - 1);
			words.erase(it);
			return 1;
		}
		int deleteWordByString(std::string s = "")
		{// with clean
			std::map<std::string, int>::iterator it = words.find(s);
			if (it != words.end())
			{ words.erase(it); this->cleanBadWords(false); return 1; }
			else { return 0; }
		}
		bool compareLanguage(Language* q)
		{// to compare languages need use it
			if (this->count != q->getCount()) return false;
			else if (!ABC->compareAlphabet(q->getAlphabet())) return false;
			else return std::equal(this->words.begin(), this->words.end(), 
				q->getWords().begin());
		}
	};

	// many types of prefix of language
	namespace sgpri_m2
	{
		Language* getPrefixLang(std::string s, bool o = true, bool nullword = false);
		Language* getPrefixLang(Language* l, bool o = true, bool nullword = false);
		Language* getSuffixLang(std::string s, bool o = true, bool nullword = false);
		Language* getSuffixLang(Language* l, bool o = true, bool nullword = false);
	}

	// cover relationship
	char coverR_option(std::string option);
	Language* choosePrefSuffLang(Language* l, std::string option);
	/* option: relation of B to A;
		original version of Phi is B follow uA, so "dogonyat" | "uA>=B" | "BfollowA" are acceptable;
		another version is B covers uA, so "peregonyat" | "uA<=B" | "BcoverA" are acceptable. */
	Language* Phi_AminusB(Language* la, Language* lb, std::string u, std::string option = "BfollowA");
	Language* Phi_AminusB(Language* la, Language* lb, Language* lc, std::string option = "BfollowA");

	// functions of combinatorics
	std::vector<int*> myArrangeNum(int all, int choose, bool repeatElem);
	std::vector<int*> myArrangeNum_noRe(int all, int choose);
	std::vector<int*> myCombinationNum(int all, int choose, bool repeatElem);
	std::vector<int*> myCmbntnN_noRe(int all, int choose);
	std::vector<std::pair<int, std::vector<int*>>> mySubsetNum(int all);

	// set of sublanguages
	std::pair<
		std::vector<std::pair<int, std::vector<int*>>>,
		std::vector<Language*>> getAllSublang(Language* lan);
	int findLanguageInSubLangs(Language* l, const std::pair<
		std::vector<std::pair<int, std::vector<int*>>>, std::vector<Language*>>&subl);
	int findLanguageInSubLangs(Language* l, const std::vector<Language*>& subl,
		const std::vector<std::pair<int, std::vector<int*>>>& m);
	int findLanguageInSubLangs(Language* l, const std::vector<Language*>& subl);

	// table of automaton PRI
	std::pair<
		std::pair<std::vector<Language*>, std::vector<Language*>>,
		int*> getTablePRI_twoLang(
			Language* la, Language* lb, std::string option = "BfollowA", bool onlyAttainable = true);
	void getTablePRI_twoLang_printProgram(
		std::pair<std::pair<std::vector<Language*>, std::vector<Language*>>, int*> test,
		char len = 'l', std::string ep = "_");

	// change the order of parametres
	Language* operationSemigroup_F(
		Language* base, Language* x, Language* y,
		std::string option = "BfollowA");

	// table of groupoid(magma)
	std::pair<
		std::pair<Language*, std::vector<Language*>>,
		std::pair<int, int*>> tableMagma_allSubsetOPrefBaseLanguage_opsg(
			Language* bl, std::string option = "BfollowA");
	void tableMagma_aSsOPBL_opsg_print(
		std::pair<std::pair<Language*, std::vector<Language*>>, std::pair<int, int*>> t,
		char len = 'l', std::string ep = "_");
	std::string tableMagma_aSsOPBL_opsg_toFile(
		Language* bl, std::string filename,
		std::string option = "BfollowA", char len = 'l', std::string ep = "_");

	// comfortable method to give a language
	namespace sgpri_m2
	{
		Language* readLangFromString_s(std::string l,
			Alphabet* defaul, std::set<std::string> nw);
		Language* readLangFromString_l(std::string l,
			Alphabet* defaul, std::set<std::string> nw);
	}
	Language* readLangFromString(std::string l,
		std::set<std::string> nw = std::set<std::string>(),
		Alphabet* defaul = nullptr);
	Language* readLangFromString(std::string l,
		std::string ep,
		Alphabet* defaul = nullptr);

	// read table from file
	std::pair<
		std::pair<Language*, std::vector<Language*>>,
		std::pair<int, int*>> readTable_opsg_fromFile(
			std::string filename, Language* bl, std::string option = "BfollowA");
	std::string tablePRI_toFile(
		std::pair<std::pair<std::vector<Language*>, std::vector<Language*>>, int*> table,
		std::string filename, char len = 'l', std::string ep = "_");
	std::pair<
		std::pair<std::vector<Language*>, std::vector<Language*>>,
		int*> readTable_opsg_fromFile(
			std::string filename, std::set<std::string> nw);

	// uniform table type (may occupy more space)
	std::pair<
		std::pair<std::vector<Language*>, std::vector<Language*>>,
		int*> uniform_table(
			std::pair<
			std::pair<std::vector<Language*>, std::vector<Language*>>,
			int*>& table);
	std::pair<
		std::pair<std::vector<Language*>, std::vector<Language*>>,
		int*> uniform_table(
			std::pair<
			std::pair<Language*, std::vector<Language*>>,
			std::pair<int, int*>>&table);

	// some equvilent names, they are sugars
	using typePRI_2 = std::pair<std::pair<std::vector<Language*>, std::vector<Language*>>, int*>;
	using typePRI_1 = std::pair<std::pair<Language*, std::vector<Language*>>, std::pair<int, int*>>;
	typePRI_1 tablePRI_allSubsetOPrefBaseLanguage_opsg(Language* bl, std::string option = "BfollowA");
	void tablePRI_aSsOPBL_opsg_print(typePRI_1 t, char len = 'l', std::string ep = "_");
	std::string tablePRI_aSsOPBL_opsg_toFile(
		Language* bl, std::string filename,
		std::string option = "BfollowA", char len = 'l', std::string ep = "_");
	typePRI_1 tablePRI_groupoid(Language* bl, std::string option = "BfollowA");
	void tablePRI_groupoid_print(typePRI_1 t, char len = 'l', std::string ep = "_");
	std::string tablePRI_groupoid_toFile(Language* bl, std::string filename,
		std::string option = "BfollowA", char len = 'l', std::string ep = "_");
	using pri1_table = std::pair<int, int*>;
	using table_trans = std::pair<int, int*>;
	// in this program: groupoid<=>magma, semigroup<-pri1~PRI1->automaton.
	template<typename T> void tablePRI_print(T t, char len = 'l', std::string ep = "_");

	// important elements to semigroup
	// test-X return non-X-evidence, get-X return X-element, have-X return X-evidence
	// _pri1: for table from one base language
	// _trans: for semiautomaton
	// section: semiautomaton
	Language* lang_union(Language* a, Language* b);
	Language* lang_intersect(Language* a, Language* b);
	Language* lang_multiple(Language* a, Language* b);
	table_trans compose_trans(const table_trans& t1, const table_trans& t2);
	table_trans get_trans_fromPRI2(const typePRI_2& t, Language* l);
	table_trans get_trans_fromPRI2(const typePRI_2& t, std::string w);
	Language* get_word_byTrans_fromPRI2(const typePRI_2& t, const table_trans& m);
	// section: groupoid-pri of one base language
	std::vector<int*> testClosure_pri1(const pri1_table& table);
	std::vector<int*> testClosure_pri1(const typePRI_1& table);
	std::pair<std::vector<Language*>, std::vector<int*>> testClosure_pri1(
		Language* bl, std::string option = "BfollowA", bool loading = true);
	std::vector<int*> testAssociativity_pri1(const pri1_table& table);
	std::vector<int*> testAssociativity_pri1(const typePRI_1& table);
	std::pair<std::vector<Language*>, std::vector<int*>> testAssociativity_pri1(
		Language* bl, std::string option = "BfollowA", bool loading = true);
	std::vector<int> getTSIdentity_pri1(const pri1_table& table);
	std::vector<int> getLeftIdentity_pri1(const pri1_table& table);
	std::vector<int> getRightIdentity_pri1(const pri1_table& table);
	std::vector<int> getIdempotent_pri1(const pri1_table& table);
	std::map<int, std::pair<std::pair<int, int>,
		std::vector<int>>> getPowerSeries_pri1(const pri1_table& table);
	std::pair<int, std::vector<int*>> haveCommutativity_pri1(const pri1_table& table);
	bool  binaryRelation_22s1i(pri1_table p, int q1, int q2, int q3, int q4, int a);
	bool  binaryRelation_22s(pri1_table p, int q1, int q2, int q3, int q4);
	std::pair<std::vector<int*>, bool*>  binaryRelation_table(pri1_table p, char prnt = '\0');
	std::string  binaryRelation_table(pri1_table p, std::string filename, char prnt = 'p');
	std::pair<std::vector<int*>, bool*>  readBRTable_fromFile(pri1_table p, std::string filename, char prnt = '\0');
	std::vector<int*>  binaryRelation_2s2iChain(pri1_table p, int p1, int p2, int input = 1);
	std::vector<int> minSubSemiGroup_generated(pri1_table t, std::vector<int> gen);
	table_trans minSubSemiGroup_generated(pri1_table t, table_trans gen);
	std::vector<table_trans> maxSubGroupSemigroup(pri1_table t);
	std::vector<table_trans> subGroupsSemigroup(pri1_table t);
	std::vector<int> testLatinSquare_pri1(const pri1_table& table);

	// intergrated tests
	std::pair<pri1_table, std::pair<std::vector<int*>, std::vector<int*>>> testSemiGroup_pri1(
		Language* bl, std::string f, std::string option = "BfollowA", char len = 'l', std::string ep = "_");
	std::pair<pri1_table, std::pair<std::vector<int*>, std::vector<int*>>> testSemiGroup_pri1(
		Language* bl, std::string option = "BfollowA", char len = 'l', std::string ep = "_");
	std::pair<std::vector<std::string>, std::vector<std::pair<std::string,
		clock_t>>> testSemiGroup_pri1_str(Language* bl,
			std::string option = "BfollowA", char len = 'l', std::string ep = "_");
	void testSemiGroup_pri1_print(Language* bl,
		std::string option = "BfollowA", char len = 'l', std::string ep = "_");
	void testSemiGroup_pri1_toFile(Language* bl, std::string f, bool prnt = true,
		std::string option = "BfollowA", char len = 'l', std::string ep = "_");
	std::map<std::string, table_trans> intergrated_research_process(
		Language* bl, std::string testname, std::map<std::string, table_trans> items,
		std::string option = "BfollowA", char len = 'l', std::string ep = "_");
	void research_example();

	// non-determ version
	using typeNSPRI_2 = std::pair<std::pair<std::vector<Language*>, std::vector<Language*>>, int**>;
	using typeNSPRI_1 = std::pair<std::vector<Language*>, int**>;
	//using nspri_table = std::pair<std::pair<Language*, Language*>, int**>; // useless
	std::pair<
		std::pair<std::vector<Language*>, std::vector<Language*>>,
		int**> getTableNSPRI_twoLang(
			Language* la, Language* lb, std::string option = "BfollowA", bool onlyAttainable = true);
	void getTableNSPRI_printProgram(
		std::pair<std::pair<std::vector<Language*>, std::vector<Language*>>, int**> test,
		char len = 'l', std::string ep = "_");
	std::pair<std::vector<Language*>, int**> getTableNSPRI_oneLang(
		Language* lb, std::string option = "BfollowA");
	void getTableNSPRI_printProgram(
		std::pair<std::vector<Language*>, int**> test,
		char len = 'l', std::string ep = "_");
	std::string tableNSPRI_toFile(
		std::pair<std::pair<std::vector<Language*>, std::vector<Language*>>, int**> test,
		std::string filename, char len = 'l', std::string ep = "_");
	std::string tableNSPRI_toFile(std::pair<std::vector<Language*>, int**> test,
		std::string filename, char len = 'l', std::string ep = "_");
	typeNSPRI_2 readTableNSPRI_fromFile(std::string filename, std::set<std::string> nw);
	typePRI_2 getPRI_fromNSPRI(typeNSPRI_2 t);
	pri1_table getPRI_fromNSPRI(typeNSPRI_1 t);
	typePRI_1 getPRI_fromNSPRI(typeNSPRI_1 t, Language* b);
	std::map<std::string, table_trans> intergrated_research_process_NS(
		Language* bl, std::string testname, std::map<std::string, table_trans> items,
		std::string option = "BfollowA", char len = 'l', std::string ep = "_");
	void research_example_ns();

	// seem like relating to semilattice & parallel computation
	std::pair<std::pair<std::vector<Language*>, std::vector<Language*>>,
		std::vector<int* >> opsgConservatedJoinTriple(typePRI_2 t,
			const std::pair<std::vector<std::pair<int, std::vector<int*>>>,
			std::vector<Language*>>&subl,
			bool re = true);
	std::pair<std::pair<std::vector<Language*>, std::vector<Language*>>,
		std::vector<int*>> opsgConservatedJoinTriple(typePRI_2 t, bool re = true);
	std::pair<std::pair<std::vector<Language*>, std::vector<Language*>>,
		std::vector<int*>> opsgConservatedJoinTriple(typePRI_2 t, Language* b, bool re = true);

	//void sgpri_m3_Documentation(std::string s = "");
	namespace sgpri_m2
	{
		void test_1()
		{
			//test: getPrefixLang
			std::cout << getPrefixLang("abcd", false, false)->toStringLang() << std::endl;
			std::cout << getPrefixLang("abcd", false, true)->toStringLang() << std::endl;
			std::cout << getPrefixLang("abcd", true, false)->toStringLang() << std::endl;
			std::cout << getPrefixLang("abcd", true, true)->toStringLang() << std::endl;
		}
		void test_2()
		{
			//test: Phi_AminusB
			Alphabet* ab = new Alphabet("ab");
			std::cout << ab->toStringLetters() << std::endl;
			Language* a = new Language(ab);
			a->addWord_("ab");
			a->addWord_("abb");
			a->addWord_("aaa");
			a->cleanBadWords(false);
			Language* b = new Language(ab);
			b->addWord_("ba");
			b->addWord_("ab");
			b->cleanBadWords(false);
			std::cout << Phi_AminusB(a, b, "baab", "BfollowA")->toStringLang() << std::endl;
			std::cout << Phi_AminusB(a, b, "baab", "BcoverA")->toStringLang() << std::endl;
			Language* c = new Language(ab);
			c->addWord_("baab");
			c->addWord_("abb");
			c->cleanBadWords(false);
			std::cout << Phi_AminusB(a, b, c, "BfollowA")->toStringLang() << std::endl;
			std::cout << Phi_AminusB(a, b, c, "BcoverA")->toStringLang() << std::endl;
		}
		void test_3()
		{
			//test: combination
			std::vector<int*> a53 = myArrangeNum(5, 3, true);
			for (int* e : a53)
			{
				for (int i = 0; i < 3; i++) std::cout << e[i] << ',';
				std::cout << '\t';
			}
			a53 = myArrangeNum(5, 3, false);
			for (int* e : a53)
			{
				for (int i = 0; i < 3; i++) std::cout << e[i] << ',';
				std::cout << '\t';
			}
			a53 = myArrangeNum_noRe(5, 3);
			for (int* e : a53)
			{
				for (int i = 0; i < 3; i++) std::cout << e[i] << ',';
				std::cout << '\t';
			}
			std::vector<int*> c53 = myCombinationNum(5, 3, true);
			for (int* e : c53)
			{
				for (int i = 0; i < 3; i++) std::cout << e[i] << ',';
				std::cout << '\t';
			}
			std::cout << std::endl << std::endl;
			c53 = myCmbntnN_noRe(5, 3);
			for (int* e : c53)
			{
				for (int i = 0; i < 3; i++) std::cout << e[i] << ',';
				std::cout << '\t';
			}
			std::cout << std::endl << std::endl;
			auto ssn5 = mySubsetNum(5);
			for (auto& e : ssn5)
			{
				std::cout << e.first << std::endl;
				for (auto& ee : e.second)
				{
					for (int i = 0; i < e.first; i++)
					{
						std::cout << ee[i] << ',';
					}
					std::cout << std::endl;
				}
			}
			std::cout << std::endl << std::endl;
			Alphabet* ab = new Alphabet("ab");
			Language* a = new Language(ab);
			a->addWord_("ab");
			a->addWord_("abb");
			a->addWord_("aaa");
			a->cleanBadWords(false);
			auto sl = getAllSublang(getPrefixLang(a, true, true));
			for (auto& e : sl.second)
			{
				std::cout << e->toStringLanguage() << std::endl;
			}
		}
		void test_4()
		{
			//test: table PRI
			Alphabet* ab = new Alphabet("ab");
			Language* a = new Language(ab);
			a->addWord_("ab");
			a->addWord_("abb");
			a->addWord_("aaa");
			a->cleanBadWords(false);
			Language* b = new Language(ab);
			b->addWord_("ba");
			b->addWord_("ab");
			b->cleanBadWords(false);
			getTablePRI_twoLang_printProgram(getTablePRI_twoLang(a, b, "peregonyat", true));
			std::cout << std::endl << std::endl;
			getTablePRI_twoLang_printProgram(getTablePRI_twoLang(a, b, "peregonyat", false));
			a = new Language(ab);
			a->addWord_("aaa");
			a->addWord_("aabba");
			a->addWord_("abba");
			a->addWord_("bb");
			a->cleanBadWords(false);
			b = new Language(ab);
			b->addWord_("aaaa");
			b->addWord_("abb");
			b->addWord_("abba");
			b->addWord_("bbb");
			b->cleanBadWords(false);
			getTablePRI_twoLang_printProgram(getTablePRI_twoLang(b, a, "peregonyat", true), 's', "_");
		}
		void test_5()
		{
			//test: tableMagma
			Alphabet* ab = new Alphabet("ab");
			Language* b = new Language(ab);
			b->addWord_("abb");
			b->cleanBadWords(false);
			auto res = tableMagma_allSubsetOPrefBaseLanguage_opsg(b);
			tableMagma_aSsOPBL_opsg_print(res, 's');
		}
		void test_6()
		{
			//test: read language
			Language* t = readLangFromString("{aa,aba,bb,bc}");
			t = readLangFromString("{k,_,ab}", "_"); // exists null word
			t = readLangFromString("{ }", " "); // a null word
			t = readLangFromString("{k,,ab}"); // exists null word
			t = readLangFromString("{,}"); // a null word
			t = readLangFromString("//Language[4]{ <aa:1> <aba:2> <bb:3> <bc:4><:2> }");
			//Language[5]{ <:1> <aa:2> <aba:3> <bb:4> <bc:5> }
			std::cout << t->toStringLanguage() << std::endl;
		}
		void test_7()
		{
			//test: read table
			Language* b = readLangFromString("{abc}");
			tableMagma_aSsOPBL_opsg_toFile(b, "test.csv");
			auto res1 = readTable_opsg_fromFile("test.csv", b, "BfollowA");
			tableMagma_aSsOPBL_opsg_print(res1);
			Language* a = readLangFromString("{aaa,aba,bbb}");
			b = readLangFromString("{ab,ba}");
			tablePRI_toFile(getTablePRI_twoLang(a, b), "test2.csv");
			auto res2 = readTable_opsg_fromFile("test2.csv", { "_" });
			getTablePRI_twoLang_printProgram(res2);
		}
		void test_8()
		{
			//test: map...
			std::map<int, std::string> t1;
			t1[0] = "1";
			t1[1] = "2";
			std::cout << t1.size() << std::endl;
			std::cout << ((t1.find(2) != t1.end()) ? (t1[2]) : ("not")) << std::endl;
			std::cout << t1.size() << std::endl;
			std::cout << t1[2] << std::endl;
			std::cout << t1.size() << std::endl;

			//test: begin()
			std::vector<int> t2 = { 0,1,2,3 };
			std::cout << *(t2.begin()) << std::endl; //0
			std::cout << *(t2.cbegin()) << std::endl; //0
			std::cout << *(t2.rbegin()) << std::endl; //3
		}
		void test_9()
		{
			//test: example
			research_example();
		}
		void test_10()
		{
			//test: NSPRI
			Alphabet* ab = new Alphabet("ab");
			Language* a = new Language(ab);
			a->addWord_("ab");
			a->addWord_("abb");
			a->addWord_("aaa");
			a->cleanBadWords(false);
			Language* b = new Language(ab);
			b->addWord_("ba");
			b->addWord_("ab");
			b->cleanBadWords(false);
			getTableNSPRI_printProgram(getTableNSPRI_twoLang(a, b, "BfollowA", false), 'l', "_");
			std::cout << std::endl;
			tableNSPRI_toFile(getTableNSPRI_oneLang(a), "test_nspri.csv");
			typeNSPRI_2 t = readTableNSPRI_fromFile("test_nspri.csv", { "_" });
			getTableNSPRI_printProgram(t, 's', "_");
			getTableNSPRI_printProgram(getTableNSPRI_oneLang(a, "BfollowA"), 's', "_");
			std::cout << std::endl;
			tableMagma_aSsOPBL_opsg_print(tableMagma_allSubsetOPrefBaseLanguage_opsg(a), 's');
		}
		void test_11()
		{
			Alphabet* ab = new Alphabet("ab");
			Language* a = new Language(ab);
			a->addWord_("ab");
			a->addWord_("abb");
			a->addWord_("aaa");
			a->cleanBadWords(false);
			typeNSPRI_1 t = getTableNSPRI_oneLang(a);
			getTableNSPRI_printProgram(t, 's');
			pri1_table tt = getPRI_fromNSPRI(t);
			std::cout << std::endl << tt.first << std::endl;
			//
			Language* b = new Language(ab);
			b->addWord_("bbb");
			b->cleanBadWords(false);
			typeNSPRI_1 t2 = getTableNSPRI_oneLang(b);
			getTableNSPRI_printProgram(t2, 's');
			pri1_table tt2 = getPRI_fromNSPRI(t2);
			std::cout << std::endl << tt2.first << std::endl;
			//
			research_example_ns();
		}
		void test_12()
		{
			std::cout << getSuffixLang(readLangFromString("{abcd}"), true, true)->toStringLanguage() << std::endl;
			Language* b = readLangFromString("{aaaa,abb,abba,bbb}");
			Language* a = readLangFromString("{aaa,aabba,abba,bb}");
			tablePRI_print(getTablePRI_twoLang(a, b, "BcoverA", true), 's', "_");
			std::cout << "next" << std::endl;
			auto beg = clock();
			auto t = getTablePRI_twoLang(a, b, "BcoverA", false);
			auto fin = clock();
			std::cout << fin - beg << " ms\t" << "next" << std::endl;
			for (int i = 0; i < t.first.first.size(); i++)
			{
				for (int j = 0; j < t.first.second.size(); j++)
				{
					if (t.second[i * t.first.second.size() + j] < 0)
					{
						std::cout << "(" << i << "," << j << ") ";
					}
				}
			}
			std::cout << std::endl << "next" << std::endl;
			getTableNSPRI_printProgram(getTableNSPRI_twoLang(
				a, b, "BcoverA", false), 's', "_");
		}
		void test_13()
		{
			Language* a = readLangFromString("{aaa,aabba,abba,bb}");
			Language* b = readLangFromString("{aaaa,abb,abba,bbb}");
			std::cout << a->toStringLang() << b->toStringLang() << std::endl;
			typePRI_2 t = getTablePRI_twoLang(a, b, "BfollowA", true);
			tablePRI_print(t, 's');
			std::vector<int*> r = opsgConservatedJoinTriple(t, false).second;
			std::cout << r.size() << " triples" << std::endl;
			for (int* e : r)
			{
				std::cout << "col= " << e[0] << ", lines={ " << e[1] << ", " << e[2] << " }\t";
			}
			std::cout << std::endl;
			r = opsgConservatedJoinTriple(t, b, false).second;
			std::cout << r.size() << " triples" << std::endl;
			for (int* e : r)
			{
				std::cout << "col= " << e[0] << ", lines={ " << e[1] << ", " << e[2] << " }\t";
			}
			std::cout << std::endl;
		}
	}
	void test_()
	{
		sgpri_m2::test_13();
		return;
	}

	// to do list:
	// not finished (not in intergrated research yet):
	// // // maxSubGroupSemigroup, subGroupsSemigroup, testLatinSquare_pri1. 
	// other new

	// many types of prefix of language
	namespace sgpri_m2
	{
		Language* getPrefixLang(std::string s, bool o, bool nullword)
		{// return a language of prefixes of provided string
			// if parameter "o" == true, 
			// then result won't have string itself, 
			// otherwise it will
			// if "nullword" == true, 
			// then result will have empty word Epsilon, 
			// otherwise it won't
			Alphabet* abc = new Alphabet(s);
			Language* pref = new Language(abc);
			std::string tmp = s;
			if (o) tmp.pop_back();
			while (tmp.length() > 0)
			{
				pref->addWord_(tmp);
				tmp.pop_back();
			}
			if (nullword) pref->addWord_(tmp);
			pref->cleanBadWords();
			return pref;
		}
		Language* getPrefixLang(Language* l, bool o, bool nullword)
		{// return a language of all prefixes of words of provided language
			// "o" <=> won't have string itself
			// "nullword" <=> will have empty word Epsilon
			Language* pref = new Language(l->getAlphabet());
			l->cleanBadWords(false);
			for (int i = 1; i <= l->getCount(); i++)
			{
				std::string t = l->getWord_afterCleaning(i);
				for (int j = 1; o ? (j < t.length()) : (j <= t.length()); j++)
				{ pref->addWord(t.substr(0, j), false); }
			}
			if (nullword) pref->addWord_("");
			pref->cleanBadWords();
			return pref;
		}
		Language* getSuffixLang(std::string s, bool o, bool nullword)
		{// return a language of all suffixes of provided string
			// "o" <=> won't have string itself
			// "nullword" <=> will have empty word Epsilon
			Alphabet* abc = new Alphabet(s);
			Language* suff = new Language(abc);
			std::string tmp = s;
			if (o) tmp = tmp.substr(1);
			while (tmp.length() > 0)
			{
				suff->addWord_(tmp);
				tmp = tmp.substr(1);
			}
			if (nullword) suff->addWord_(tmp);
			suff->cleanBadWords();
			return suff;
		}
		Language* getSuffixLang(Language* l, bool o, bool nullword)
		{// return a language of suffixes of words of provided language
			// "o" <=> won't have string itself
			// "nullword" <=> will have empty word Epsilon
			Language* suff = new Language(l->getAlphabet());
			l->cleanBadWords(false);
			for (int i = 1; i <= l->getCount(); i++)
			{
				std::string t = l->getWord_afterCleaning(i);
				for (int j = o ? 1 : 0; j < t.length(); j++)
				{ suff->addWord(t.substr(j), false); }
			}
			if (nullword) suff->addWord_("");
			suff->cleanBadWords();
			return suff;
		}
	}

	// cover relationship
	char coverR_option(std::string option)
	{// to save us from forgetful inner representation, 
		// all functions accept instruction such as dogonyat(russian), BfollowA(english), uA>=B(formula)
		// this function turns it to inner representation 'd' or 'p'
		// good for potential further extension
		if (option == "dogonyat" || option == "BfollowA" || option == "uA>=B") return 'd';
		if (option == "peregonyat" || option == "BcoverA" || option == "uA<=B") return 'p';
		return '0';
	}
	Language* choosePrefSuffLang(Language* l, std::string option)
	{// 2 modes of automaton correspond prefixes and suffixes
		if (coverR_option(option) == 'd') return sgpri_m2::getPrefixLang(l, true, true);
		if (coverR_option(option) == 'p') return sgpri_m2::getSuffixLang(l, false, true);
		return nullptr;
	}
	/* option: relation of B to A;
		original version of Phi is B follow uA, so "dogonyat" | "uA>=B" | "BfollowA" are acceptable;
		another version is B covers uA, so "peregonyat" | "uA<=B" | "BcoverA" are acceptable. */
	Language* Phi_AminusB(Language* la, Language* lb, std::string u, std::string option)
	{// it works completely same as algorithm in the article
		la->cleanBadWords(false);
		lb->cleanBadWords(false);
		Language* D_res = new Language(lb->getAlphabet());
		Language* D_help = new Language(lb->getAlphabet());
		Language* opref_B = sgpri_m2::getPrefixLang(lb, true, true);
		for (int i = 1; i <= la->getCount(); i++)
			D_help->addWord_(u + la->getWord_afterCleaning(i));
		int flag = 1;
		while (flag)
		{
			D_help->cleanBadWords(false);
			if (D_help->getCount() == 0) break;
			std::string v = D_help->getWord_afterCleaning(1);
			D_help->deleteWordByPosition(1);
			// step 4 in original algorithm
			for (int i = 1; i <= lb->getCount(); i++)
			{
				std::string tmp_u = lb->getWord_afterCleaning(i);
				if (v.substr(0, tmp_u.length()) == tmp_u)
					D_help->addWord_(v.substr(tmp_u.length(), v.length()));
			}
			// step 5 in original algorithm
			if (opref_B->askWordInLang(v))
			{
				if (coverR_option(option) == 'd')
					D_res->addWord(v, true);
				else if (coverR_option(option) == 'p')
				{
					for (int i = 1; i <= lb->getCount(); i++)
					{
						std::string w = lb->getWord_afterCleaning(i);
						if (w.substr(0, v.length()) == v)
							D_res->addWord(w.substr(v.length(), w.length()), true);
					}
				}
			}
		}
		delete opref_B;
		delete D_help;
		D_res->cleanBadWords(false);
		return D_res;
	}
	Language* Phi_AminusB(Language* la, Language* lb, Language* lc, std::string option)
	{// it uses former function on every word of language "lc" then get union of results
		Language* res = new Language(lb->getAlphabet());
		lc->cleanBadWords(false);
		for (int i = 1; i <= lc->getCount(); i++)
		{
			auto tmp_q = Phi_AminusB(la, lb, lc->getWord_afterCleaning(i), option);
			tmp_q->cleanBadWords(false);
			for (int j = 1; j <= tmp_q->getCount(); j++) res->addWord_(tmp_q->getWord_afterCleaning(j));
			delete tmp_q;
		}
		return res;
	}

	// functions of combinatorics
	// they don't calculate any amount, they make list of integers to represent every situation
	// additionally, we call their result as model_SOMETHING_number
	std::vector<int*> myArrangeNum(int all, int choose, bool repeatElem)
	{// it generates arrangements of integers
		// arrange k (="choose") from n (="all") nonrepetitive => 
		// => result will have n!/(n-k)! pointers, every one of them is for an array with k integers
		// "repeatElem" == true <=> allow repeat elements
		std::vector<int*> res;
		int* hold = new int[choose];
		if (repeatElem)
		{
			for (int i = 0; i < choose; i++) hold[i] = 0;
		}
		else
		{
			for (int i = 0; i < choose; i++) hold[i] = i;
		}
		bool totalFlag = true;
		while (totalFlag)
		{
			if (!repeatElem)
			{
				std::set<int> tmps;
				for (int i = 0; i < choose; i++) tmps.insert(hold[i]);
				if (tmps.size() == choose)
				{
					int* tmp = new int[choose];
					for (int i = 0; i < choose; i++) tmp[i] = hold[i];
					res.push_back(tmp);
				}
				std::set<int>().swap(tmps);
			}
			else
			{
				int* tmp = new int[choose];
				for (int i = 0; i < choose; i++) tmp[i] = hold[i];
				res.push_back(tmp);
			}
			hold[choose - 1]++;
			for (int i = choose - 1; i > 0; i--)
			{
				if (hold[i] == all)
				{
					hold[i] = 0;
					hold[i - 1]++;
				}
			}
			if (hold[0] == all) totalFlag = false;
		}
		delete[] hold;
		return res;
	}
	std::vector<int*> myArrangeNum_noRe(int all, int choose)
	{// it also generates arrangements of number as former function without checking repetitivity
		// but implemented not very smartly...
		std::vector<int*> res;
		int* hold = new int[choose];
		int* nore = new int[all];
		for (int i = 0; i < choose; i++)
		{
			hold[i] = i;
			nore[i] = 0;
		}
		for (int i = choose; i < all; i++)
		{
			nore[i] = 0;
		}
		int totalFlag = all - choose + 1;
		for (int i = all - choose + 2; i <= all; i++)
		{
			totalFlag *= i;
		}
		while (totalFlag)
		{
			bool flag = true;
			for (int i = 0; i < choose; i++)
			{
				if (nore[hold[i]])
				{
					flag = false;
					break;
				}
				else
				{
					nore[hold[i]] = 1;
				}
			}
			if (flag)
			{
				int* tmp = new int[choose];
				for (int i = 0; i < choose; i++) tmp[i] = hold[i];
				res.push_back(tmp);
				totalFlag -= 1;
			}
			hold[choose - 1] += 1;
			for (int i = choose - 1; i > 0; i--)
			{
				if (hold[i] == all)
				{
					hold[i] = 0;
					hold[i - 1]++;
				}
			}
			for (int i = 0; i < all; i++)
			{
				nore[i] = 0;
			}
		}
		delete[] hold;
		return res;
	}
	std::vector<int*> myCombinationNum(int all, int choose, bool repeatElem)
	{// it generates combinations of integers
		// combine k (="choose") from n (="all") nonrepetitive => 
		// => result will have n!/((n-k)!k!) pointers, every one of them is for an array with k integers
		// "repeatElem" == true <=> allow repeat elements
		std::vector<int*> res;
		int* hold = new int[choose];
		if (repeatElem)
		{
			for (int i = 0; i < choose; i++) hold[i] = 0;
		}
		else
		{
			for (int i = 0; i < choose; i++) hold[i] = i;
		}
		bool totalFlag = true;
		while (totalFlag)
		{
			if (!repeatElem)
			{
				std::set<int> tmps;
				for (int i = 0; i < choose; i++) tmps.insert(hold[i]);
				if (tmps.size() == choose)
				{
					int* tmp = new int[choose];
					for (int i = 0; i < choose; i++) tmp[i] = hold[i];
					res.push_back(tmp);
				}
				std::set<int>().swap(tmps);
			}
			else
			{
				int* tmp = new int[choose];
				for (int i = 0; i < choose; i++) tmp[i] = hold[i];
				res.push_back(tmp);
			}
			hold[choose - 1]++;
			bool carryFlag = false;
			for (int i = choose - 1; i > 0; i--)
			{
				if (hold[i] == all)
				{
					carryFlag = true;
					hold[i] = 0;
					hold[i - 1]++;
				}
			}
			if (carryFlag)
				for (int i = 0; i < choose - 1; i++)
				{
					if (hold[i + 1] < hold[i]) hold[i + 1] = hold[i];
				}
			if (hold[0] == all) totalFlag = false;
		}
		delete[] hold;
		return res;
	}
	std::vector<int*> myCmbntnN_noRe(int all, int choose)
	{// it also generates combinations of integers as former function without checking repetitivity
		// so it has less loops
		std::vector<int*> res;
		if (all <= 0 || choose <= 0 || choose > all) return res;
		int* hold = new int[choose];
		for (int i = 0; i < choose; i++) hold[i] = i;
		while (hold[0] != all - choose)
		{
			if (hold[choose - 1] >= all)
			{
				int tmpflag = choose - 1;
				while (hold[tmpflag] - hold[tmpflag - 1] == 1) tmpflag--;
				hold[tmpflag - 1]++;
				for (int i = tmpflag; i < choose; i++)
					hold[i] = hold[i - 1] + 1;
				continue;
			}
			int* tmp = new int[choose];
			for (int i = 0; i < choose; i++) tmp[i] = hold[i];
			res.push_back(tmp);
			hold[choose - 1]++;
		}
		res.push_back(hold);
		return res;
	}
	std::vector<std::pair<int, std::vector<int*>>> mySubsetNum(int all)
	{// it generates all possible subsets of sets of first n (="all") natural numbers
		// obviously it uses former function
		// in result: size=all, every element=<how many from_all, these combinations are>
		std::vector<std::pair<int, std::vector<int*>>> res;
		for (int i = 0; i <= all; i++)
		{
			res.push_back(std::make_pair(i, myCmbntnN_noRe(all, i)));
		}
		return res;
	}

	// set of sublanguages
	std::pair< std::vector<std::pair<int, std::vector<int*>>>,
		std::vector<Language*>> getAllSublang(Language* lan)
	{// it generates all sublanguages of provided language
		//result is: 
		// <model_subset_number aka results of function "mySubsetNum" (in case that it's needed later), 
		// vector of pointers of sublanguages>
		lan->cleanBadWords(false);
		std::vector<Language*> res;
		Alphabet* abc = lan->getAlphabet();
		Language* nullLang = new Language(abc);
		res.push_back(nullLang);
		auto subsetModel = mySubsetNum(lan->getCount());
		if (lan->getCount() < 1) return std::make_pair(subsetModel, res);
		auto vecIntStr = lan->getVecStr_withClean();
		for (auto& e : subsetModel)
		{
			if (e.first == lan->getCount()) res.push_back(lan);
			else for (auto& ee : e.second)
			{
				Language* tmplan = new Language(abc);
				for (int i = 0; i < e.first; i++)
					tmplan->addWord_(vecIntStr[ee[i]]);
				tmplan->cleanBadWords(false);
				res.push_back(tmplan);
			}
		}
		return std::make_pair(subsetModel, res);
	}
	int findLanguageInSubLangs(Language* l, 
		const std::pair< std::vector<std::pair<int, std::vector<int*>>>, 
			std::vector<Language*>>& subl)
	{// implemented this function to find index
		// because in work remarked that 
		// always need to find index of some sublanguages
		// to save time it uses model_subset_number, 
		// which should be returned from function "getAllSublang"
		l->cleanBadWords(false);
		int cnt = l->getCount(), b = 0;
		for (auto& e : subl.first)
		{
			if (e.first != cnt) b += e.second.size();
			else
			{
				cnt = b + e.second.size();
				break;
			}
		}
		for (; b < cnt; b++)
			if (l->compareLanguage(subl.second[b]))
				return b;
		return -1;
	}
	int findLanguageInSubLangs(Language* l, 
		const std::vector<Language*>& subl,
		const std::vector<std::pair<int, std::vector<int*>>>& m)
	{// an overloading implementation of finding index
		l->cleanBadWords(false);
		int cnt = l->getCount(), b = 0;
		for (auto& e : m)
		{
			if (e.first != cnt) b += e.second.size();
			else
			{
				cnt = b + e.second.size();
				break;
			}
		}
		for (; b < cnt; b++)
		{
			if (l->compareLanguage(subl[b]))
				return b;
		}
		return -1;
	}
	int findLanguageInSubLangs(Language* l, 
		const std::vector<Language*>& subl)
	{// an overloading implementation of finding index
		// in case that we don't have model_subset_number, 
		// thus, it's slower
		l->cleanBadWords(false);
		int cnt = l->getCount();
		for (int b = 0; b < subl.size(); b++)
		{
			if (l->compareLanguage(subl[b]))
				return b;
		}
		return -1;
	}

	// table of automaton PRI
	std::pair<
		std::pair<std::vector<Language*>, std::vector<Language*>>,
		int*> getTablePRI_twoLang(
			Language* la, Language* lb, std::string option, bool onlyAttainable)
	{// it makes table of automaton PRI for 2 languages
		// in return: 
		// < <list of languages for lines, list of languages for columns>, 
		// pointer for table >
		// table is an array of integers, which are indexes of lines-languages, therefore, 
		// pointer[i * (length of list of languages for columns) + j] = 
		// the index (in the list of languages for lines) of the result of PHI on 
		// the i-th language in list of languages for lines and the j-th languages in list of languages for columns
		la->cleanBadWords(false);
		lb->cleanBadWords(false);
		std::vector<Language*> asl;
		asl = getAllSublang(choosePrefSuffLang(lb, option)).second;
		asl.erase(asl.begin()); //nullLang
		asl.erase(asl.begin()); //beginLang
		std::vector<Language*> stt;
		std::vector<Language*> newLang;
		Language* nullLang = new Language(lb->getAlphabet());
		stt.push_back(nullLang);
		Language* beginLang = new Language(lb->getAlphabet());
		beginLang->addWord_("");
		beginLang->cleanBadWords(false);
		stt.push_back(beginLang);
		newLang.push_back(beginLang);
		std::vector<Language*> AbyWord;
		for (int i = 1; i <= la->getCount(); i++)
		{
			Language* tmp = new Language(la->getAlphabet());
			tmp->addWord_(la->getWord_afterCleaning(i));
			tmp->cleanBadWords(false);
			AbyWord.push_back(tmp);
		}
		std::vector<int*> tablePRI;
		int* tmp = new int[AbyWord.size()];
		for (int i = 0; i < AbyWord.size(); i++)
			tmp[i] = 0;
		tablePRI.push_back(tmp);
		auto it = newLang.begin();
		while (it != newLang.end())
		{
			tmp = new int[AbyWord.size()];
			for (int i = 0; i < AbyWord.size(); i++)
			{
				tmp[i] = -1;
				Language* tmpL = Phi_AminusB(AbyWord[i], lb, *it, option);
				tmpL->cleanBadWords(false);
				int findflag = 0;
				for (; findflag < stt.size(); findflag++)
				{
					if (stt[findflag]->compareLanguage(tmpL))
					{
						tmp[i] = findflag;
						break;
					}
				}
				if (findflag == stt.size())
				{
					for (auto j = asl.begin(); j != asl.end(); j++)
					{
						if ((*j)->compareLanguage(tmpL))
						{
							asl.erase(j);
							break;
						}
					}
					stt.push_back(tmpL);
					newLang.push_back(tmpL);
					it = newLang.begin();
					for (; findflag < stt.size(); findflag++)
					{
						if (stt[findflag]->compareLanguage(tmpL))
						{
							tmp[i] = findflag;
							break;
						}
					}
				}
			}
			tablePRI.push_back(tmp);
			it = newLang.erase(it);
		}
		if (!onlyAttainable)
		{
			for (it = asl.begin(); it != asl.end(); it++)
			{
				newLang.push_back(*it);
				stt.push_back(*it);
			}
			it = newLang.begin();
			while (newLang.size())
			{
				tmp = new int[AbyWord.size()];
				for (int i = 0; i < AbyWord.size(); i++)
				{
					tmp[i] = -1;
					Language* tmpL = Phi_AminusB(AbyWord[i], lb, *it, option);
					tmpL->cleanBadWords(false);
					for (int j = 0; j < stt.size(); j++)
					{
						if (stt[j]->compareLanguage(tmpL))
						{
							tmp[i] = j;
							break;
						}
					}
				}
				tablePRI.push_back(tmp);
				it = newLang.erase(it);
			}
		}
		int* tablePRI_res = new int[tablePRI.size() * AbyWord.size()];
		for (int i = 0; i < tablePRI.size(); i++)
		{
			for (int j = 0; j < AbyWord.size(); j++)
				tablePRI_res[i * AbyWord.size() + j] = tablePRI[i][j];
		}
		return std::make_pair(std::make_pair(stt, AbyWord), tablePRI_res);
	}
	void getTablePRI_twoLang_printProgram(
		std::pair<std::pair<std::vector<Language*>, std::vector<Language*>>, int*> test,
		char len, std::string ep)
	{// it prints the table (result of former function "getTablePRI_twoLang") on console
		// when parameter "len"=='l', 
		// string for representing languages will longer
		// provide parameter "ep" to decide 
		// what string will be used for representing empty word epsilon
		int w1 = 0, w2 = 0;
		if (len == 'l')
		{ for (auto& e : test.first.first) 
			if (e->toStringLanguage().length() > w1) 
				w1 = e->toStringLanguage().length(); }
		else
		{ for (auto& e : test.first.first) 
			if (e->toStringLang(ep).length() > w1) 
				w1 = e->toStringLang(ep).length(); }
		if (len == 'l')
		{ for (auto& e : test.first.second) 
			if (e->toStringLanguage().length() > w2) 
				w2 = e->toStringLanguage().length(); }
		else
		{ for (auto& e : test.first.second) 
			if (e->toStringLang(ep).length() > w2) 
				w2 = e->toStringLang(ep).length(); }
		std::cout << std::left << std::setw(w1) << "";
		if (len == 'l')
		{ for (auto& e : test.first.second) 
			std::cout << std::left << std::setw(w2) << e->toStringLanguage(); }
		else
		{ for (auto& e : test.first.second) 
			std::cout << std::left << std::setw(w2) << e->toStringLang(ep); }
		std::cout << std::endl;
		for (int i = 0; i < test.first.first.size(); i++)
		{
			if (len == 'l')
				std::cout << std::left << std::setw(w1) 
				<< test.first.first[i]->toStringLanguage();
			else
				std::cout << std::left << std::setw(w1) 
				<< test.first.first[i]->toStringLang(ep); 
			for (int j = 0; j < test.first.second.size(); j++)
				std::cout << std::left << std::setw(w2) 
				<< test.second[i * test.first.second.size() + j]; 
			std::cout << std::endl;
		}
	}

	// change the order of parametres
	Language* operationSemigroup_F(
		Language* base, Language* x, Language* y, std::string option)
	{// just change order of parameters
		return Phi_AminusB(y, base, x, option);
	}

	// table of groupoid(magma)
	// MAGMA is a relatively new name for groupoid (algebric structure), it's just shorter.
	std::pair<
		std::pair<Language*, std::vector<Language*>>,
		std::pair<int, int*>> tableMagma_allSubsetOPrefBaseLanguage_opsg(
			Language* bl, std::string option)
	{// it makes table of groupoid/automaton PRI for 1 language
		// to help later test, return <
		// < base language, list of all sublanguages of OPref > , 
		// <size of one side of square table, table> >
		// mostly, not commutative, so can't just use half of tableSG
		Language* _fix_bl = choosePrefSuffLang(bl, option);
		auto sublan_FIXBL_pair = getAllSublang(_fix_bl);
		std::vector<Language*> sublanFIXBL = sublan_FIXBL_pair.second;
		std::vector<std::pair<int, std::vector<int*>>> subsetmodel = sublan_FIXBL_pair.first;
		int tableSize = sublanFIXBL.size();
		int* tableSG = new int[tableSize * tableSize]();
		for (int i = 0; i < tableSize; i++)
		{
			for (int j = 0; j < tableSize; j++)
			{
				tableSG[i * tableSize + j] = -1;
				Language* afterOperation = operationSemigroup_F(bl, sublanFIXBL[i], sublanFIXBL[j], option);
				afterOperation->cleanBadWords(false);
				int tmprank = 0, tmpcnt = 0, aftopsize = afterOperation->getCount();
				for (tmpcnt = 0; tmpcnt < subsetmodel.size(); tmpcnt++)
				{
					if (subsetmodel[tmpcnt].first == aftopsize) break;
					tmprank += subsetmodel[tmpcnt].second.size();
				}
				if (tmpcnt == subsetmodel.size())
					continue;
				else for (int k = tmprank; k <= subsetmodel[tmpcnt].second.size() + tmprank; k++)
				{
					if (sublanFIXBL[k]->compareLanguage(afterOperation))
					{
						tableSG[i * tableSize + j] = k;
						break;
					}
				}
				delete afterOperation;
			}
		}
		return std::make_pair(std::make_pair(bl, sublanFIXBL), std::make_pair(tableSize, tableSG));
	}
	void tableMagma_aSsOPBL_opsg_print(
		std::pair<std::pair<Language*, std::vector<Language*>>, std::pair<int, int*>> t,
		char len, std::string ep)
	{// sugar
		// prints table on console
		getTablePRI_twoLang_printProgram(
			std::make_pair(std::make_pair(t.first.second, t.first.second), t.second.second),
			len, ep);
	}
	std::string tableMagma_aSsOPBL_opsg_toFile(
		Language* bl, std::string filename,
		std::string option, char len, std::string ep)
	{// it prints table into file
		std::fstream output_fstream;
		output_fstream.open(filename, std::ios_base::out);
		if (!output_fstream.is_open())
		{
			std::cout << "tableMagma_allSubsetOPrefBaseLanguage_opsg can't open file\n";
			return "";
		}
		Language* _fix_bl = choosePrefSuffLang(bl, option);
		auto sublanFIXBL_pair = getAllSublang(_fix_bl);
		std::vector<Language*> sublanFIXBL = sublanFIXBL_pair.second;
		std::vector<std::pair<int, std::vector<int*>>> subsetmodel = sublanFIXBL_pair.first;
		int tableSize = sublanFIXBL.size();
		int tableSG = -1;
		output_fstream << ";";
		for (int i = 0; i < tableSize; i++)
		{
			if (len == 'l') output_fstream << sublanFIXBL[i]->toStringLanguage() << ";";
			else output_fstream << sublanFIXBL[i]->toStringLang(ep) << ";";
		}
		output_fstream << std::endl;
		output_fstream.close();
		std::cout << tableSize << std::endl;//loading bar
		for (int i = 0; i < tableSize; i++)
		{
			output_fstream.open(filename, std::ios_base::app);
			if (len == 'l') output_fstream << sublanFIXBL[i]->toStringLanguage() << ";";
			else output_fstream << sublanFIXBL[i]->toStringLang(ep) << ";";
			for (int j = 0; j < tableSize; j++)
			{
				tableSG = -1;
				Language* afterOperation = operationSemigroup_F(
					bl, sublanFIXBL[i], sublanFIXBL[j], option);
				afterOperation->cleanBadWords(false);
				int tmprank = 0, tmpcnt = 0, aftopsize = afterOperation->getCount();
				for (tmpcnt = 0; tmpcnt < subsetmodel.size(); tmpcnt++)
				{
					if (subsetmodel[tmpcnt].first == aftopsize) break;
					tmprank += subsetmodel[tmpcnt].second.size();
				}
				if (tmpcnt == subsetmodel.size())
				{
					continue;
				}
				else for (int k = tmprank; k <= subsetmodel[tmpcnt].second.size() + tmprank; k++)
				{
					if (sublanFIXBL[k]->compareLanguage(afterOperation))
					{
						tableSG = k;
						break;
					}
				}
				output_fstream << tableSG << ";";
				delete afterOperation;
			}
			output_fstream << std::endl;
			output_fstream.close();
			if (i % 10 == 0) std::cout << i << " ";//loading bar
		}
		//output_fstream.close();
		std::cout << std::endl;//loading bar
		return filename;
	}

	// comfortable method to give a language
	namespace sgpri_m2
	{
		Language* readLangFromString_s(std::string l,
			Alphabet* defaul, std::set<std::string> nw)
		{// it reads a language from short representation string of it
			// parameter "nw" provides all representations for empty word Epsilon
			// but in short representation empty word Epsilon causes confusion, see below situations:
			// ("{k,_,ab}","_") => exists null word
			// ("{ }"," ") => a null word
			// ("{k,,ab}") => exists null word
			// ("{,}") => a null word
			l = l.substr(1, l.length() - 2);
			if (l.length() == 0)
			{
				if (defaul == nullptr)
				{
					defaul = new Alphabet("ab");
				}
				Language* lang = new Language(defaul);
				return lang;
			}
			Language* res = new Language();
			while (true)
			{
				auto pos = l.find_first_of(',');
				if (pos == std::string::npos)
				{
					if (nw.find(l) != nw.end())
					{
						res->addWord("", false);
					}
					else
					{
						res->addWord(l, false);
					}
					break;
				}
				else
				{
					if (nw.find(l.substr(0, pos)) != nw.end())
						res->addWord("", false);
					else
						res->addWord(l.substr(0, pos), false);
					l = l.substr(pos + 1);
				}
			}
			return res;
		}
		Language* readLangFromString_l(std::string l,
			Alphabet* defaul, std::set<std::string> nw)
		{// it reads a language from string representation of it
			// parameter "nw" provides all representations for empty word Epsilon
			// in long representation everything is clear
			// if (l.substr(l.find_first_of('['), 2) == "[0")
			if (l.find_first_of('<') == std::string::npos)
			{
				if (defaul == nullptr)
				{
					defaul = new Alphabet("ab");
				}
				Language* lang = new Language(defaul);
				return lang;
			}
			Language* res = new Language();
			while (true)
			{
				auto pos1 = l.find_first_of('<');
				auto pos2 = l.find_first_of(':');
				if (pos1 == std::string::npos)
				{
					break;
				}
				else
				{
					if (nw.find(l.substr(pos1 + 1, pos2 - pos1 - 1)) != nw.end())
					{
						res->addWord("", false);
					}
					else
					{
						res->addWord(l.substr(pos1 + 1, pos2 - pos1 - 1), false);
					}
					l = l.substr(pos2 + 1);
				}
			}
			return res;
		}
	}
	Language* readLangFromString(std::string l,
		std::set<std::string> nw, Alphabet* defaul)
	{// it reads a language from a string, 
		// it recognizes whether provided string is short or long representation
		// parameter "nw" provides all representations for empty word Epsilon
		Alphabet* defa;
		if (defaul == nullptr)
		{ defa = new Alphabet("ab"); }
		else
		{ defa = defaul; }
		if (l[0] == '{')
			return sgpri_m2::readLangFromString_s(l, defa, nw);
		else
			return sgpri_m2::readLangFromString_l(l, defa, nw);
	}
	Language* readLangFromString(std::string l,
		std::string ep, Alphabet* defaul)
	{// it reads a language from a string, it recognizes whether provided string is short or long representation
		// parameter "nw" provides one representation for empty word Epsilon
		std::set<std::string> epp = { ep };
		Alphabet* defa;
		if (defaul == nullptr)
		{
			defa = new Alphabet("ab");
		}
		else
		{
			defa = defaul;
		}
		if (l[0] == '{')
			return sgpri_m2::readLangFromString_s(l, defa, epp);
		else
			return sgpri_m2::readLangFromString_l(l, defa, epp);
	}

	// read table from file
	std::pair<
		std::pair<Language*, std::vector<Language*>>,
		std::pair<int, int*>> readTable_opsg_fromFile(
			std::string filename, Language* bl, std::string option)
	{// it reads a table of groupoid PRI from file
		// it makes list of languages by using baseLanguage 
		// (tolerable in our situation)
		// without function for reading language from string
		// to help later tests, return <
		// < base language, vector of all sublanguages of opref > , 
		// <size of one side of square table OR ERRORCODE, table> >
		std::fstream fs;
		fs.open(filename, std::ios_base::in);
		if (!fs.is_open())
		{
			std::cout << "readTable_opsg_fromFile(file,lang) can't open file\n";
			return std::make_pair(
				std::make_pair(nullptr, std::vector<Language*>()),
				std::make_pair(-8, nullptr));
		}
		Language* _fix_bl = choosePrefSuffLang(bl, option);
		auto sublanFIXBL_pair = getAllSublang(_fix_bl);
		std::vector<Language*> sublanFIXBL = sublanFIXBL_pair.second;
		std::vector<std::pair<int, std::vector<int*>>> subsetmodel = sublanFIXBL_pair.first;
		//int tableSize = sublanOprfxBl.size(); // not very safe
		int i = 0, j = 0, tableSize = sublanFIXBL.size(), err = 0;
		std::vector<std::vector<int>> tableSG;
		std::string tmpLine;
		std::getline(fs, tmpLine);
		std::getline(fs, tmpLine);
		//for (int i = 0; i < tableSize; i++) //not very safe
		while (true)
		{
			tmpLine = tmpLine.substr(tmpLine.find_first_of(';') + 1);
			if (tmpLine.empty()) break;
			//for (int j = 0; j < tableSize; j++) //not very safe
			j = 0;
			std::vector<int> tmpvector;
			while (!tmpLine.empty())
			{
				//tableSG[i * tableSize + j] = ( // not very safe
				tmpvector.push_back(
					(
						atoi(tmpLine.substr(0, tmpLine.find_first_of(';')).c_str()) == 0
						&&
						tmpLine.substr(0, tmpLine.find_first_of(';')) != "0"
						) ? (-1) : atoi(tmpLine.substr(0, tmpLine.find_first_of(';')).c_str())
				);
				tmpLine = tmpLine.substr(tmpLine.find_first_of(';') + 1);
				if (j >= tableSize) err |= 1; // columns too many
				j += 1;
			}
			tableSG.push_back(tmpvector);
			std::getline(fs, tmpLine);
			if (i >= tableSize) err |= 2; // lines too many
			if (i > j) err |= 4; // lines more than columns
			i += 1;
		}
		fs.close();
		if (err) return std::make_pair(make_pair(nullptr, std::vector<Language*>()), std::make_pair(-err, nullptr));
		int* tableSG_res = new int[tableSize * tableSize];
		i = 0;
		for (std::vector<int> e : tableSG)
		{
			j = 0;
			for (int ee : e)
			{ tableSG_res[i * tableSize + j] = ee; j += 1; }
			i += 1;
			e.clear();
		}
		tableSG.clear();
		return std::make_pair(
			std::make_pair(bl, sublanFIXBL), 
			std::make_pair(tableSize, tableSG_res));
	}
	std::string tablePRI_toFile(
		std::pair<std::pair<std::vector<Language*>, std::vector<Language*>>, int*> table,
		std::string filename, char len, std::string ep)
	{// it prints a table of groupoid PRI into file
		std::fstream output_fstream;
		output_fstream.open(filename, std::ios_base::out);
		if (!output_fstream.is_open())
		{
			std::cout << "tablePRI_toFile can't open file\n";
			return "";
		}
		std::vector<Language*> langCol = table.first.second;
		std::vector<Language*> langLine = table.first.first;
		output_fstream << ";";
		for (Language* e : langCol)
		{
			if (len == 'l') output_fstream << e->toStringLanguage() << ";";
			else output_fstream << e->toStringLang(ep) << ";";
		}
		output_fstream << std::endl;
		output_fstream.close();
		for (int i = 0; i < langLine.size(); i++)
		{
			output_fstream.open(filename, std::ios_base::app);
			if (len == 'l') output_fstream << langLine[i]->toStringLanguage() << ";";
			else output_fstream << langLine[i]->toStringLang(ep) << ";";
			for (int j = 0; j < langCol.size(); j++)
			{
				output_fstream << table.second[i * langCol.size() + j] << ";";
			}
			output_fstream << std::endl;
			output_fstream.close();
		}
		return filename;
	}
	std::pair<
		std::pair<std::vector<Language*>, std::vector<Language*>>,
		int*> readTable_opsg_fromFile(
			std::string filename, std::set<std::string> nw)
	{// it reads a table of groupoid PRI from file
		// it reads languages from files directly
		// return < < languages-lines, languages-columns > , table >
		std::fstream fs;
		fs.open(filename, std::ios_base::in);
		if (!fs.is_open())
		{
			std::cout << "readTable_opsg_fromFile(file,lang) can't open file\n";
			return std::make_pair(
				std::make_pair(std::vector<Language*>(), std::vector<Language*>()),
				nullptr);
		}
		std::vector<Language*> langLine, langCol;
		std::vector<std::vector<int>> tableSG;
		int j = 0, col = 0;
		std::string tmpLine;
		std::getline(fs, tmpLine);
		while (!tmpLine.empty())
		{
			if (tmpLine.substr(0, tmpLine.find_first_of(';')).empty())
			{
				tmpLine = tmpLine.substr(tmpLine.find_first_of(';') + 1);
				while (!tmpLine.empty())
				{
					std::string tmps = tmpLine.substr(0, tmpLine.find_first_of(';'));
					Language* tmpl = readLangFromString(tmps, nw);
					langCol.push_back(tmpl);
					tmpLine = tmpLine.substr(tmpLine.find_first_of(';') + 1);
				}
			}
			else
			{
				std::string tmps = tmpLine.substr(0, tmpLine.find_first_of(';'));
				Language* tmpl = readLangFromString(tmps, nw);
				langLine.push_back(tmpl);
				tmpLine = tmpLine.substr(tmpLine.find_first_of(';') + 1);
				std::vector<int> tmpvector;
				j = 0;
				while (!tmpLine.empty())
				{
					tmpvector.push_back(
						(
							atoi(tmpLine.substr(0, tmpLine.find_first_of(';')).c_str()) == 0
							&&
							tmpLine.substr(0, tmpLine.find_first_of(';')) != "0"
							) ? (-1) : atoi(tmpLine.substr(0, tmpLine.find_first_of(';')).c_str())
					);
					tmpLine = tmpLine.substr(tmpLine.find_first_of(';') + 1);
					j += 1;
				}
				if (!col)
				{
					col = j;
				}
				else if (j > col)
				{
					col = -1; // line length changed
					break;
				}
				tableSG.push_back(tmpvector);
			}
			std::getline(fs, tmpLine);
			if (tmpLine.empty()) break;
		}
		fs.close();
		if (col < 0 || col != langCol.size())
			return std::make_pair(make_pair(std::vector<Language*>(), std::vector<Language*>()), nullptr);
		int* tableSG_res = new int[langLine.size() * langCol.size()];
		col = 0;
		for (std::vector<int> e : tableSG)
		{
			j = 0;
			for (int ee : e)
			{
				tableSG_res[col * langCol.size() + j] = ee;
				j += 1;
			}
			col += 1;
			e.clear();
		}
		tableSG.clear();
		return std::make_pair(make_pair(langLine, langCol), tableSG_res);
	}

	// uniform table type (may occupy more space)
	// these functions transform data structure for groupoid PRI into data structure for automaton PRI
	// later there will be more data structures, these functions are not very useful
	std::pair<
		std::pair<std::vector<Language*>, std::vector<Language*>>,
		int*> uniform_table(
			std::pair<
			std::pair<std::vector<Language*>, std::vector<Language*>>,
			int*>& table)
	{// overloading version for those when there's no need to transform
		return table;
	}
	std::pair<
		std::pair<std::vector<Language*>, std::vector<Language*>>,
		int*> uniform_table(
			std::pair<
			std::pair<Language*, std::vector<Language*>>,
			std::pair<int, int*>>&table)
	{
		// < <base language, sublanguages>, <table size, table> > -> 
		// -> < <sublanguages, sublanguages>, table >
		return std::make_pair(std::make_pair(table.first.second, table.first.second), table.second.second);
	}

	// some equvilent names, they are sugars
	typePRI_1 tablePRI_allSubsetOPrefBaseLanguage_opsg(Language* bl, std::string option)
	{
		return tableMagma_allSubsetOPrefBaseLanguage_opsg(bl, option);
	}
	void tablePRI_aSsOPBL_opsg_print(typePRI_1 t, char len, std::string ep)
	{
		getTablePRI_twoLang_printProgram(uniform_table(t), len, ep);
	}
	std::string tablePRI_aSsOPBL_opsg_toFile(
		Language* bl, std::string filename,
		std::string option, char len, std::string ep)
	{
		return tableMagma_aSsOPBL_opsg_toFile(bl, filename, option, len, ep);
	}
	typePRI_1 tablePRI_groupoid(Language* bl, std::string option)
	{
		return tableMagma_allSubsetOPrefBaseLanguage_opsg(bl, option);
	}
	void tablePRI_groupoid_print(typePRI_1 t, char len, std::string ep)
	{
		getTablePRI_twoLang_printProgram(uniform_table(t), len, ep);
	}
	std::string tablePRI_groupoid_toFile(Language* bl, std::string filename,
		std::string option, char len, std::string ep)
	{
		return tableMagma_aSsOPBL_opsg_toFile(bl, filename, option, len, ep);
	}
	template<typename T> void tablePRI_print(T t, char len, std::string ep)
	{
		getTablePRI_twoLang_printProgram(uniform_table(t), len, ep);
	}
	// in this program names of functions have logic like: 
	// groupoid<=>magma, 
	// semigroup<-pri1~PRI1->automaton 
	// (name with capital letters is more about automaton, but they are also used for groupoid since data structures are same)


	/*important elements to semigroup
	test-X return non-X-evidence, get-X return X-element, have-X return X-evidence
	_pri1: for table from one base language
	_trans: for semiautomaton*/

	// section: semiautomaton
	// any input letter is equivalent to a mapping on the set of states
	// use array to represent semiautomaton=mapping: 
	// in array the i-th element is j means this input transforms the i-th state to the j-th state
	Language* lang_union(Language* a, Language* b)
	{// it returns the union of 2 languages
		if (a == nullptr && b == nullptr) return nullptr;
		else if (a == nullptr)
		{
			Language* res = new Language(b);
			return res;
		}
		else if (b == nullptr)
		{
			Language* res = new Language(a);
			return res;
		}
		Language* res = new Language(a->getAlphabet());
		for (std::string& e : a->getVecStr_withClean()) res->addWord(e, false);
		for (std::string& e : b->getVecStr_withClean()) res->addWord(e, false);
		res->cleanBadWords(false);
		return res;
	}
	Language* lang_intersect(Language* a, Language* b)
	{// it returns intersect of 2 languages
		if (a == nullptr && b == nullptr) return nullptr;
		else if (a == nullptr) return (new Language(b->getAlphabet()));
		else if (b == nullptr) return (new Language(a->getAlphabet()));
		Language* res = new Language(a->getAlphabet());
		for (std::string& e : a->getVecStr_withClean())
			if (b->askWordInLang(e)) res->addWord(e, false);
		res->cleanBadWords(false);
		return res;
	}
	Language* lang_multiple(Language* a, Language* b)
	{// it returns concatenation‌/product of 2 languages
		if (a == nullptr && b == nullptr) return nullptr;
		else if (a == nullptr) return (new Language(b->getAlphabet()));
		else if (b == nullptr) return (new Language(a->getAlphabet()));
		Language* res = new Language();
		for (std::string& e : a->getVecStr_withClean())
		{
			for (std::string& ee : b->getVecStr_withClean())
			{ res->addWord(e + ee, false); }
		}
		res->cleanBadWords(false);
		return res;
	}
	table_trans compose_trans(const table_trans& t1, const table_trans& t2)
	{// it returns composition of 2 mapping
		// result is equivalent to a mapping 
		// formed by the concatenation of 2 letters
		if (t1.first != t2.first)
		{ return std::make_pair(-1, nullptr); }
		int* res = new int[t1.first]();
		for (int i = 0; i < t1.first; i++)
		{ res[i] = t2.second[t1.first]; }
		return std::make_pair(t1.first, res);
	}
	table_trans get_trans_fromPRI2(const typePRI_2& t, Language* l)
	{// it takes out the mapping 
		// formed by input "l" from table of automaton PRI "t"
		int i = 0;
		for (i = 0; i < t.first.second.size(); i++)
		{
			if (t.first.second[i]->compareLanguage(l))
			{
				int* res = new int[t.first.first.size()];
				for (int j = 0; j < t.first.first.size(); j++)
				{ res[j] = t.second[j * t.first.second.size() + i]; }
				return std::make_pair(t.first.second.size(), res);
			}
		}
		return std::make_pair(-1, nullptr);
	}
	table_trans get_trans_fromPRI2(const typePRI_2& t, std::string w)
	{// it takes out the mapping 
		// formed by input "w" from table of automaton PRI "t"
		int i = 0;
		for (i = 0; i < t.first.second.size(); i++)
		{
			if (t.first.second[i]->askWordInLang(w))
			{
				int* res = new int[t.first.first.size()];
				for (int j = 0; j < t.first.first.size(); j++)
				{ res[j] = t.second[j * t.first.second.size() + i]; }
				return std::make_pair(t.first.second.size(), res);
			}
		}
		return std::make_pair(-1, nullptr);
	}
	Language* get_word_byTrans_fromPRI2(const typePRI_2& t, const table_trans& m)
	{// it takes out the language, which forms the mapping "m", 
		// from table of automaton PRI "t"
		Language* res = nullptr;
		if (t.first.first.size() == m.first)
		{
			for (int i = 0; i < t.first.second.size(); i++)
			{
				bool flag = true;
				for (int j = 0; j < t.first.first.size(); j++)
				{
					if (m.second[j] != t.second[j * t.first.second.size() + i])
					{ flag = false; break; }
				}
				if (flag)
				{
					Language* tmp = lang_union(res, t.first.second[i]);
					if (res != nullptr) delete res;
					res = tmp;
				}
			}
		}
		return res;
	}
	// section: groupoid-pri of one base language
	std::vector<int*> testClosure_pri1(const pri1_table& table)
	{// it tests whether table of automaton PRI has element beyond the set of states
		// which is theoretically impossible (automaton PRI of 1 language IS a groupoid)
		// though, we formally do it
		std::vector<int*> res;
		for (int i = 0; i < table.first; i++)
		{
			for (int j = 0; j < table.first; j++)
			{
				if (table.second[i * table.first + j] < 0)
				{
					res.push_back(new int[2] {i, j});
				}
			}
		}
		return res;
	}
	std::vector<int*> testClosure_pri1(const typePRI_1& table)
	{// test closurement, overloading for another data structure
		return testClosure_pri1(table.second);
	}
	std::pair<std::vector<Language*>, std::vector<int*>> testClosure_pri1(
		Language* bl, std::string option, bool loading)
	{// test closurement, special slow version
		std::pair<
			std::vector<std::pair<int, std::vector<int*>>>,
			std::vector<Language*>> subl = getAllSublang(choosePrefSuffLang(bl, option));
		std::vector<int*> res;
		if (loading) std::cout << "testClosure_pri1(slow) began, loading bar: " << subl.second.size() << std::endl;
		for (int i = 0; i < subl.second.size(); i++)
		{
			for (int j = 0; j < subl.second.size(); j++)
			{
				if (findLanguageInSubLangs(
					operationSemigroup_F(
						bl, subl.second[i], subl.second[j], option),
					subl) < 0)
				{
					res.push_back(new int[2] {i, j});
				}
			}
			if (loading && !(i % 10)) std::cout << i << "\t";
		}
		if (loading) std::cout << std::endl;
		return std::make_pair(subl.second, res);
	}
	std::vector<int*> testAssociativity_pri1(const pri1_table& table)
	{// it tests whether all triples of states in the automaton PRI for 1 language are associative
		std::vector<int*> res;
		for (int i = 0; i < table.first; i++)
		{
			for (int j = 0; j < table.first; j++)
			{
				for (int k = 0; k < table.first; k++)
				{
					if (
						table.second[table.second[i * table.first + j] * table.first + k]
						!=
						table.second[i * table.first + table.second[j * table.first + k]])
					{
						res.push_back(new int[3] {i, j, k});
					}
				}
			}
		}
		return res;
	}
	std::vector<int*> testAssociativity_pri1(const typePRI_1& table)
	{// test associaticity, overloading for another data structure
		return testAssociativity_pri1(table.second);
	}
	std::pair<std::vector<Language*>, std::vector<int*>> testAssociativity_pri1(
		Language* bl, std::string option, bool loading)
	{// test associaticity, special slow version
		std::pair<
			std::vector<std::pair<int, std::vector<int*>>>,
			std::vector<Language*>> subl = getAllSublang(choosePrefSuffLang(bl, option));
		std::vector<int*> res;
		if (loading) std::cout << "testAssociativity_pri1(slow) began, loading bar: " << subl.second.size() << std::endl;
		for (int i = 0; i < subl.second.size(); i++)
		{
			for (int j = 0; j < subl.second.size(); j++)
			{
				for (int k = 0; k < subl.second.size(); k++)
				{
					if (!(
						operationSemigroup_F(bl,
							operationSemigroup_F(bl, subl.second[i], subl.second[j], option),
							subl.second[k], option)->compareLanguage(
								operationSemigroup_F(bl, subl.second[i],
									operationSemigroup_F(bl, subl.second[j], subl.second[k], option), option))
						))
					{
						res.push_back(new int[3] {i, j, k});
					}
				}
			}
			if (loading && !(i % 10)) std::cout << i << "\t";
		}
		if (loading) std::cout << std::endl;
		return std::make_pair(subl.second, res);
	}
	std::vector<int> getTSIdentity_pri1(const pri1_table& table)
	{// find (two-side) identity/unit of semigroup
		std::vector<int> unit;
		for (int i = 0; i < table.first; i++)
		{
			bool flag = true;
			for (int j = 0; j < table.first; j++)
			{ if ((table.second[i * table.first + j] != j) || (table.second[j * table.first + i] != j))
				{ flag = false; break; } }
			if (flag) { unit.push_back(i); }
		}
		return unit;
	}
	std::vector<int> getLeftIdentity_pri1(const pri1_table& table)
	{// find left identity/unit of semigroup
		std::vector<int> unit;
		for (int i = 0; i < table.first; i++)
		{
			bool flag = true;
			for (int j = 0; j < table.first; j++)
			{
				if (table.second[i * table.first + j] != j)
				{
					flag = false;
					break;
				}
			}
			if (flag)
				unit.push_back(i);
		}
		return unit;
	}
	std::vector<int> getRightIdentity_pri1(const pri1_table& table)
	{// find right identity/unit of semigroup
		std::vector<int> unit;
		for (int i = 0; i < table.first; i++)
		{
			bool flag = true;
			for (int j = 0; j < table.first; j++)
			{ if (table.second[j * table.first + i] != j)
				{ flag = false; break; } }
			if (flag) { unit.push_back(i); }
		}
		return unit;
	}
	std::vector<int> getIdempotent_pri1(const pri1_table& table)
	{// find idempotent(s) of semigroup
		std::vector<int> idem;
		for (int i = 0; i < table.first; i++)
			if (table.second[i * table.first + i] == i) idem.push_back(i);
		return idem;
	}
	std::map<int, std::pair<std::pair<int, int>,
		std::vector<int>>> getPowerSeries_pri1(const pri1_table& table)
	{// find power series of all elements of semigroup
		// map[i] = <<index,period>,series>
		std::map<int, std::pair<std::pair<int, int>, std::vector<int>>> res;
		for (int i = 0; i < table.first; i++)
		{
			std::vector<int> s;
			s.push_back(i);
			bool flag = true;
			while (flag)
			{
				s.push_back(table.second[(*s.rbegin()) * table.first + i]);
				for (int j = 0; j < s.size() - 1; j++)
				{
					if (s[j] == (*s.rbegin()))
					{
						flag = false;
						res[i] = std::make_pair(std::make_pair(j + 1, s.size() - j - 1), s);
						break;
					}
				}
			}
		}
		return res;
	}
	std::pair<int, std::vector<int*>> haveCommutativity_pri1(const pri1_table& table)
	{// find pairs of elements of semigroup with commutative property
		std::vector<int*> res; //only non-trivial pairs
		int count = 0;
		for (int i = 0; i < table.first; i++)
		{
			for (int j = i + 1; j < table.first; j++)
			{
				count += 1;
				if (table.second[i * table.first + j] == table.second[j * table.first + i])
				{
					res.push_back(new int[2] {i, j});
				}
			}
		}
		return std::make_pair(count, res);
	}
	bool  binaryRelation_22s1i(pri1_table p, int q1, int q2, int q3, int q4, int a)
	{// for the specially defined binary relation in monography
		// check whether q1--a-->q3 and q2--a-->q4
		// there is not any inspection, must check size boundary in advance to avoid a > size of table
		return ((p.second[q1 * p.first + a] == q3) && (p.second[q2 * p.first + a] == q4));
	}
	bool  binaryRelation_22s(pri1_table p, int q1, int q2, int q3, int q4)
	{// for the specially defined binary relation in monography
		// check whether exists an element a that q1--a-->q3 and q2--a-->q4
		for (int i = 0; i < p.first; i++)
		{
			if (binaryRelation_22s1i(p, q1, q2, q3, q4, i)) return true;
		}
		return false;
	}
	std::pair<std::vector<int*>, bool*>  binaryRelation_table(pri1_table p, char prnt)
	{// for the specially defined binary relation in monography
		// check all combinations of elements whether they have this relation
		// <q1,q1> prohibited
		std::vector<int*> cmbnNModel = myCmbntnN_noRe(p.first, 2);
		bool* table = new bool[cmbnNModel.size() * cmbnNModel.size()];
		if (prnt) std::cout << cmbnNModel.size() << std::endl;
		for (int i = 0; i < cmbnNModel.size(); i++)
		{
			for (int j = 0; j < cmbnNModel.size(); j++)
				table[i * cmbnNModel.size() + j] =
				binaryRelation_22s(p, cmbnNModel[i][0], cmbnNModel[i][1], cmbnNModel[j][0], cmbnNModel[j][1]);
			if (prnt && (i % 100 == 0)) std::cout << i << " ";
		}
		if (prnt) std::cout << std::endl;
		return make_pair(cmbnNModel, table);
	}
	std::string  binaryRelation_table(pri1_table p, std::string filename, char prnt)
	{// for the specially defined binary relation in monography
		// check all combinations, then prints result into file
		// <q1,q1> prohibited
		std::fstream fs;
		fs.open(filename, std::ios_base::out);
		if (!fs.is_open())
		{ std::cout << "binaryRelation_table: can't open file\n"; return ""; }
		std::vector<int*> cmbnNModel = myCmbntnN_noRe(p.first, 2);
		fs << ";";
		for (auto& e : cmbnNModel)
		{ fs << "<" << e[0] << "|" << e[1] << ">;"; }
		fs << std::endl;
		for (int i = 0; i < cmbnNModel.size(); i++)
		{
			fs << "<" << cmbnNModel[i][0] << "|" << cmbnNModel[i][1] << ">;";
			for (int j = 0; j < cmbnNModel.size(); j++)
				fs << binaryRelation_22s(p, cmbnNModel[i][0], cmbnNModel[i][1],
					cmbnNModel[j][0], cmbnNModel[j][1])
				<< ";";
			fs << std::endl;
		}
		fs.close();
		return filename;
	}
	std::pair<std::vector<int*>, bool*>  readBRTable_fromFile(
		pri1_table p, std::string filename, char prnt)
	{// for the specially defined binary relation in monography
		// reads result from file
		std::fstream fs;
		fs.open(filename, std::ios_base::in);
		if (!fs.is_open())
		{
			std::cout << "readBRTable_fromFile: can't open file\n";
			return std::make_pair(std::vector<int*>(), nullptr);
		}
		auto cmbnNModel = myCmbntnN_noRe(p.first, 2);
		bool* table = new bool[cmbnNModel.size() * cmbnNModel.size()];
		std::string tmpLine;
		fs >> tmpLine;
		for (int i = 0; i < cmbnNModel.size(); i++)
		{
			fs >> tmpLine;
			tmpLine = tmpLine.substr(tmpLine.find_first_of(';') + 1);
			for (int j = 0; j < cmbnNModel.size(); j++)
			{
				table[i * cmbnNModel.size() + j] = 
					(bool)(atoi(tmpLine.substr(0, tmpLine.find_first_of(';')).c_str()));
				tmpLine = tmpLine.substr(tmpLine.find_first_of(';') + 1);
			}
		}
		fs.close();
		return std::make_pair(cmbnNModel, table);
	}
	std::vector<int*>  binaryRelation_2s2iChain(pri1_table p, int p1, int p2, int input)
	{//seems like not useful ... // for the specially defined binary relation in monography
		// find some kind chain: from pair <"p1","p2"> under the same "input" by this relation 
		std::vector<int*> res;
		int* record = new int[p.first * p.first]();
		int cnt = static_cast<int>(p.first) *
			(static_cast<int>(p.first) - 1) / 2;
		if (p1 >= p.first || p2 >= p.first || input >= p.first || p1 == p2)
		{
			std::cout << "binaryRelation+2s2iChain: error" << std::endl;
			return std::vector<int*>();
		}
		res.push_back(new int[2] {p1, p2});
		record[p1 * p.first + p2] = 1;
		while (cnt)
		{
			p1 = p.second[p1 * p.first + input];
			p2 = p.second[p2 * p.first + input];
			if (p1 == p1)
			{
				res.push_back(nullptr);
				break;
			}
			else if (record[p1 * p.first + p2])
			{
				res.push_back(new int[2] {p1, p2});
				res.push_back(new int[2] {-1, -1});
				break;
			}
			else
			{
				res.push_back(new int[2] {p1, p2});
				record[p1 * p.first + p2] = 1;
			}
			cnt -= 1;
		}
		return res;
	}
	std::vector<int> minSubSemiGroup_generated(pri1_table t, std::vector<int> gen)
	{// in semigroup "t" find minimal subsemigroup generated by its subset "gen"
		bool flag = true;
		int* res = new int[t.first]();
		for (int e : gen) res[e] = 1;
		while (flag)
		{
			flag = false;
			for (int i = 0; i < t.first; i++)
			{
				for (int j = i; j < t.first; j++)
				{
					if (res[i] && res[j])
					{
						int tmp = t.second[res[i] * t.first + res[j]];
						if (res[tmp] == 0)
						{
							flag = true;
							res[tmp] = 1;
						}
						tmp = t.second[res[j] * t.first + res[i]];
						if (res[tmp] == 0)
						{
							flag = true;
							res[tmp] = 1;
						}
					}
				}
			}
		}
		std::vector<int> res_vec;
		for (int i = 0; i < t.first; i++)
		{
			if (res[i]) res_vec.push_back(i);
		}
		return res_vec;
	}
	table_trans minSubSemiGroup_generated(pri1_table t, table_trans gen)
	{// in semiautomaton "t" find minimal subsemigroup generated by its subset "gen"
		// the i-th element in array "gen.second" is 1 means this element in subset
		bool flag = true;
		int* res = new int[t.first]();
		for (int i = 0; i < gen.first; i++) res[i] = gen.second[i];
		while (flag)
		{
			flag = false;
			for (int i = 0; i < t.first; i++)
			{
				for (int j = i; j < t.first; j++)
				{
					if (res[i] && res[j])
					{
						int tmp = t.second[res[i] * t.first + res[j]];
						if (res[tmp] == 0)
						{
							flag = true;
							res[tmp] = 1;
						}
						tmp = t.second[res[j] * t.first + res[i]];
						if (res[tmp] == 0)
						{
							flag = true;
							res[tmp] = 1;
						}
					}
				}
			}
		}
		return std::make_pair(t.first, res);
	}
	std::vector<table_trans> maxSubGroupSemigroup(pri1_table t)
	{// find maximal (sub)group in semigroup
		std::vector<table_trans> result;
		auto subsetmodel = mySubsetNum(t.first);
		for (auto it1 = subsetmodel.rbegin(); it1 != subsetmodel.rend(); it1++)
		{
			for (auto it2 : it1->second)
			{
				int* tmp = new int[(it1->first) * (it1->first)]();
				int cnt = 0;
				int* flag = new int[t.first]();
				int* cntr = new int[t.first]();
				int* shrink = new int[t.first]();
				for (int i = 0; i < it1->first; i++)
				{
					shrink[it2[i]] = i;
				}
				for (int i = 0; i < it1->first; i++)
				{
					flag[it2[i]] = 1;
					for (int j = 0; j < it1->first; j++)
					{
						cntr[t.second[it2[i] * t.first + it2[j]]] += 1;
						tmp[cnt] = shrink[t.second[it2[i] * t.first + it2[j]]];
						cnt++;
					}
				}
				for (int i = 0; i < t.first; i++)
				{
					if ((flag[i] == 0) && (cntr[i] > 0))
					{
						cnt = 0;
						break;
					}
				}
				if (cnt > 0)
				{
					pri1_table tmpt = std::make_pair(it1->first, tmp);
					if (testAssociativity_pri1(tmpt).size() == 0)
					{
						auto tsi = getTSIdentity_pri1(tmpt);
						if (tsi.size())
						{
							int revcnt = 0;
							for (int i = 0; i < tmpt.first; i++)
							{
								for (int j = 0; j < tmpt.first; j++)
								{
									auto fi = std::find(tsi.begin(), tsi.end(), tmpt.second[i * tmpt.first + j]);
									if (
										(fi != tsi.end())
										&&
										(std::find(tsi.begin(), tsi.end(), tmpt.second[j * tmpt.first + i]) == fi)
										)
									{
										revcnt++;
										break;
									}
								}
								if (revcnt <= i) break;
							}
							if (revcnt == tmpt.first)
							{
								result.push_back(std::make_pair(t.first, flag));
							}
							else
							{
								delete[] flag;
							}
						}
					}
				}
				delete[] tmp;
				delete[] cntr;
				delete[] shrink;
			}
			if (result.size()) return result;
		}
		return result;
	}
	std::vector<table_trans> subGroupsSemigroup(pri1_table t)
	{// find maximal (sub)group in semigroup
		std::vector<table_trans> result;
		auto subsetmodel = mySubsetNum(t.first);
		for (auto it1 = subsetmodel.rbegin(); it1 != subsetmodel.rend(); it1++)
		{
			for (auto it2 : it1->second)
			{
				int* tmp = new int[(it1->first) * (it1->first)]();
				int cnt = 0;
				int* flag = new int[t.first]();
				int* cntr = new int[t.first]();
				int* shrink = new int[t.first]();
				for (int i = 0; i < it1->first; i++)
				{
					shrink[it2[i]] = i;
				}
				for (int i = 0; i < it1->first; i++)
				{
					flag[it2[i]] = 1;
					for (int j = 0; j < it1->first; j++)
					{
						cntr[t.second[it2[i] * t.first + it2[j]]] += 1;
						tmp[cnt] = shrink[t.second[it2[i] * t.first + it2[j]]];
						cnt++;
					}
				}
				for (int i = 0; i < t.first; i++)
				{
					if ((flag[i] == 0) && (cntr[i] > 0))
					{
						cnt = 0;
						break;
					}
				}
				if (cnt > 0)
				{
					pri1_table tmpt = std::make_pair(it1->first, tmp);
					if (testAssociativity_pri1(tmpt).size() == 0)
					{
						auto tsi = getTSIdentity_pri1(tmpt);
						if (tsi.size())
						{
							int revcnt = 0;
							for (int i = 0; i < tmpt.first; i++)
							{
								for (int j = 0; j < tmpt.first; j++)
								{
									auto fi = std::find(tsi.begin(), tsi.end(), tmpt.second[i * tmpt.first + j]);
									if (
										(fi != tsi.end())
										&&
										(std::find(tsi.begin(), tsi.end(), tmpt.second[j * tmpt.first + i]) == fi)
										)
									{
										revcnt++;
										break;
									}
								}
								if (revcnt <= i) break;
							}
							if (revcnt == tmpt.first)
							{
								result.push_back(std::make_pair(t.first, flag));
							}
							else
							{
								delete[] flag;
							}
						}
					}
				}
				delete[] tmp;
				delete[] cntr;
				delete[] shrink;
			}
			//if (result.size()) return result;
		}
		return result;
	}
	std::vector<int> testLatinSquare_pri1(const pri1_table& table)
	{// test whether "product table" is a latin square 
		// result has "i" if i-th line is bad and "-i" if i-th column so
		std::vector<int> res;
		for (int i = 0; i < table.first; i++)
		{
			int* flag = new int[table.first]();
			for (int j = 0; j < table.first; j++)
			{
				if (!flag[table.second[i * table.first + j]]) flag[table.second[i * table.first + j]] = 1;
				else
				{
					res.push_back(i);
					break;
				}
			}
			delete[] flag;
			flag = new int[table.first]();
			for (int j = 0; j < table.first; j++)
			{
				if (!flag[table.second[j * table.first + i]]) flag[table.second[j * table.first + i]] = 1;
				else
				{
					res.push_back(-i);
					break;
				}
			}
		}
		return res;
	}

	// intergrated tests
	std::pair<pri1_table, std::pair<std::vector<int*>, std::vector<int*>>> testSemiGroup_pri1(
		Language* bl, std::string f, std::string option, char len, std::string ep)
	{// make groupoid by language "bl", save it in file "f", read it from file "f", check its closurement & associativity
		tablePRI_groupoid_toFile(bl, f, option, len, ep);
		pri1_table table = readTable_opsg_fromFile(f, bl, option).second;
		return std::make_pair(table, std::make_pair(testClosure_pri1(table), testAssociativity_pri1(table)));
	}
	std::pair<pri1_table, std::pair<std::vector<int*>, std::vector<int*>>> testSemiGroup_pri1(
		Language* bl, std::string option, char len, std::string ep)
	{// make groupoid by language "bl", check its closurement & associativity
		pri1_table table = tablePRI_groupoid(bl, option).second;
		return std::make_pair(table, std::make_pair(testClosure_pri1(table), testAssociativity_pri1(table)));
	}
	std::pair<std::vector<std::string>, std::vector<std::pair<std::string,
		clock_t>>> testSemiGroup_pri1_str(Language* bl,
			std::string option, char len, std::string ep)
		// return < vector of anti semigroup information, vector of test info pair<which part, how long it takes> >
	{// make groupoid, check its closurement & associativity, return duration of key steps
		// 4 * info_print
		std::vector<std::pair<std::string, clock_t>> test_info;
		std::vector<std::string> antisg_info;
		std::cout << "Preparation...";//loading bar
		clock_t beg, fin;
		beg = clock();
		Language* _fix_bl = choosePrefSuffLang(bl, option);
		fin = clock();
		test_info.push_back(std::make_pair("time: get oprefix/suffix Language", fin - beg));
		beg = clock();
		auto sublanFIXBL_pair = getAllSublang(_fix_bl);
		fin = clock();
		test_info.push_back(std::make_pair("time: get all sublanguages", fin - beg));
		std::vector<Language*> sublanFIXBL = sublanFIXBL_pair.second;
		int tableSize = sublanFIXBL.size();
		beg = clock();
		Language* afterOperation = operationSemigroup_F(bl,
			sublanFIXBL[tableSize - 1], sublanFIXBL[tableSize - 1], option);
		fin = clock();
		test_info.push_back(std::make_pair("time: example for one semigroup operation", fin - beg));
		std::cout << "done" << std::endl;//loading bar
		std::cout << "Making pri groupoid table...";//loading bar
		beg = clock();
		typePRI_1 tableM = tablePRI_groupoid(bl, option);
		fin = clock();
		test_info.push_back(std::make_pair(
			"time: make pri groupoid table of base language", fin - beg));
		std::cout << "done" << std::endl;//loading bar
		std::cout << "Checking closure...";//loading bar
		beg = clock();
		if (tableM.second.first != tableSize)
		{
			antisg_info.push_back("deadly error: tableM.second.first != tableSize");
			return make_pair(antisg_info, test_info);
		}
		std::string testCl_str = "";
		int antiCl_cnt = 0;
		for (int i = 0; i < tableSize; i++)
			for (int j = 0; j < tableSize; j++)
				if (tableM.second.second[i * tableSize + j] == -1)
				{
					antiCl_cnt += 1;
					//testCl_str += "(" + std::to_string(i) + "," + std::to_string(j) + ")"; // no space for rbegin
					testCl_str += "(" + std::to_string(i) + "," + std::to_string(j) + ") ";
				}
		fin = clock();
		test_info.push_back(std::make_pair("time: check closure", fin - beg));
		//if (*(testCl_str.rbegin()) == ')')
		if (antiCl_cnt)
		{
			testCl_str = "result of operation on these "
				+ std::to_string(antiCl_cnt)
				+ " pairs exceeded boundary : " + testCl_str;
			antisg_info.push_back(testCl_str);
			return make_pair(antisg_info, test_info);
		}
		std::cout << "done" << std::endl;//loading bar
		std::cout << "Checking associativity...";//loading bar
		beg = clock();
		std::vector<int*> antiAssoc_arr = testAssociativity_pri1(tableM);
		fin = clock(); std::cout << std::endl;
		test_info.push_back(std::make_pair("time: testAssociativity_pri1", fin - beg));
		std::string testA_str = "";
		if (antiAssoc_arr.size())
		{
			for (auto e : antiAssoc_arr)
			{
				testA_str += "(" + std::to_string(e[0]) + "," + std::to_string(e[1]) + "," + std::to_string(e[2]) + ") ";
			}
			testA_str = std::to_string(antiAssoc_arr.size()) + " triples have no associativity: " + testA_str;
			antisg_info.push_back(testA_str);
		}
		std::cout << "done" << std::endl;//loading bar
		return make_pair(antisg_info, test_info);
	}
	void testSemiGroup_pri1_print(Language* bl,
		std::string option, char len, std::string ep)
	{// make groupoid, check its closurement & associativity, print duration of key steps
		clock_t beg, fin;
		std::cout << "time: get oprefix/suffix Language";
		beg = clock();
		Language* _fix_bl = choosePrefSuffLang(bl, option);
		fin = clock();
		std::cout << " = " << fin - beg << std::endl;
		std::cout << "time: get all sublanguages";
		beg = clock();
		auto sublanFIXBL_pair = getAllSublang(_fix_bl);
		fin = clock();
		std::cout << " = " << fin - beg << std::endl;
		std::vector<Language*> sublanFIXBL = sublanFIXBL_pair.second;
		int tableSize = sublanFIXBL.size();
		std::cout << "time: example for one semigroup operation";
		beg = clock();
		Language* afterOperation = operationSemigroup_F(bl,
			sublanFIXBL[tableSize - 1], sublanFIXBL[tableSize - 1], option);
		fin = clock();
		std::cout << " = " << fin - beg << std::endl;
		std::cout << "time: make pri groupoid table of base language";
		beg = clock();
		typePRI_1 tableM = tablePRI_groupoid(bl, option);
		fin = clock();
		std::cout << " = " << fin - beg << std::endl;
		beg = clock();
		if (tableM.second.first != tableSize)
		{
			std::cout << "deadly error: tableM.second.first != tableSize" << std::endl;
			return;
		}
		std::cout << "result of operation on these pairs exceeded boundary: ";
		int antiCl_cnt = 0;
		for (int i = 0; i < tableSize; i++)
			for (int j = 0; j < tableSize; j++)
				if (tableM.second.second[i * tableSize + j] == -1)
				{
					antiCl_cnt += 1;
					std::cout << "(" << i << "," << j << ")";
				}
		fin = clock();
		std::cout << std::endl << "(number of anti closure pairs: " << antiCl_cnt << ")" << std::endl;
		std::cout << "time: check closure" << " = " << fin - beg << std::endl;
		std::cout << std::endl << "time: testAssociativity_pri1";
		beg = clock();
		std::vector<int*> antiAssoc_arr = testAssociativity_pri1(tableM);
		fin = clock();
		std::cout << " = " << fin - beg << std::endl;
		std::cout << "triples have no associativity: ";
		for (auto e : antiAssoc_arr)
		{
			std::cout << "(" << e[0] << "," << e[1] << "," << e[2] << ") ";
		}
		std::cout << std::endl << "(number of anti associativity triples: " << antiAssoc_arr.size() << ")" << std::endl;
		return;
	}
	void testSemiGroup_pri1_toFile(Language* bl, std::string f, bool prnt,
		std::string option, char len, std::string ep)
	{// make groupoid & save in file, check its closurement & associativity, save duration of key steps in file & may print as wish
		std::fstream fstrm;
		fstrm.open(f, std::ios_base::out);
		if (!fstrm.is_open())
		{
			std::cout << "can't open file\n";
			return;
		}
		clock_t beg, fin;
		fstrm << "time: get oprefix/suffix Language";
		if (prnt) std::cout << "get oprefix/suffix Language...";
		beg = clock();
		Language* _fix_bl = choosePrefSuffLang(bl, option);
		fin = clock();
		fstrm << " = " << fin - beg << std::endl;
		if (prnt) std::cout << "done" << std::endl;
		fstrm << "time: get all sublanguages";
		if (prnt) std::cout << "get all sublanguages...";
		beg = clock();
		auto sublanFIXBL_pair = getAllSublang(_fix_bl);
		fin = clock();
		fstrm << " = " << fin - beg << std::endl;
		if (prnt) std::cout << "done" << std::endl;
		std::vector<Language*> sublanFIXBL = sublanFIXBL_pair.second;
		int tableSize = sublanFIXBL.size();
		fstrm << "time: example for one semigroup operation";
		if (prnt) std::cout << "example for one semigroup operation...";
		beg = clock();
		Language* afterOperation = operationSemigroup_F(bl,
			sublanFIXBL[tableSize - 1], sublanFIXBL[tableSize - 1], option);
		fin = clock();
		fstrm << " = " << fin - beg << std::endl;
		if (prnt) std::cout << "done" << std::endl;
		fstrm << "time: make pri groupoid table of base language";
		if (prnt) std::cout << "make pri groupoid table of base language...";
		beg = clock();
		tablePRI_groupoid_toFile(bl, f + "_testSemiGroupPri1ToFile-Table_" + f, option, len, ep);
		fin = clock();
		//typePRI_1 tableM = tablePRI_groupoid(bl, option);
		typePRI_1 tableM = readTable_opsg_fromFile(f + "_testSemiGroupPri1ToFile-Table_" + f, bl, option);
		fstrm << " = " << fin - beg << std::endl;
		if (prnt) std::cout << "done" << std::endl;
		if (tableM.second.first != tableSize)
		{
			fstrm << "deadly error: tableM.second.first != tableSize" << std::endl;
			std::cout << "deadly error: tableM.second.first != tableSize" << std::endl;
			fstrm.close();
			return;
		}
		fstrm << "result of operation on these pairs exceeded boundary: ";
		if (prnt) std::cout << "Checking closure...";
		int antiCl_cnt = 0;
		beg = clock();
		for (int i = 0; i < tableSize; i++)
			for (int j = 0; j < tableSize; j++)
				if (tableM.second.second[i * tableSize + j] == -1)
				{
					antiCl_cnt += 1;
					fstrm << "(" << i << "," << j << ")";
				}
		fin = clock();
		fstrm << std::endl << "(number of anti closure pairs: " << antiCl_cnt << ")" << std::endl;
		if (prnt) std::cout << "done" << std::endl;
		if (prnt) std::cout << "(number of anti closure pairs: " << antiCl_cnt << ")" << std::endl;
		fstrm << std::endl << "time: check closure" << " = " << fin - beg << std::endl << std::endl;
		if (prnt) std::cout << "Checking associativity...";
		beg = clock();
		std::vector<int*> antiAssoc_arr = testAssociativity_pri1(tableM);
		fin = clock();
		fstrm << "triples have no associativity: ";
		for (auto e : antiAssoc_arr)
		{
			fstrm << "(" << e[0] << "," << e[1] << "," << e[2] << ") ";
		}
		fstrm << std::endl << "(number of anti associativity triples: " << antiAssoc_arr.size() << ")" << std::endl;
		if (prnt) std::cout << "done" << std::endl;
		if (prnt) std::cout << "(number of anti associativity triples: " << antiAssoc_arr.size() << ")" << std::endl;
		fstrm << "time: testAssociativity_pri1" << " = " << fin - beg << std::endl;
		fstrm.close();
		return;
	}
	std::map<std::string, table_trans> intergrated_research_process(
		Language* bl, std::string testname, std::map<std::string, table_trans> items,
		std::string option, char len, std::string ep)
	{// a very comprehensive research on a groupoid
		// parameter "items" provides items, which need to be researched
		// how to provide them: see function "research_example" below
		clock_t beg, fin;
		std::fstream fstrm;
		fstrm.open(testname + "_researchOnSemigroup_mainInfo.txt", std::ios_base::out);
		if (!fstrm.is_open())
		{
			std::cout << "intergrated_research_process: can't open file _researchOnSemigroup\n";
			return std::map<std::string, table_trans>();
		}
		std::map<std::string, table_trans> ret;
		// introduction
		if (len == 'l')
		{
			fstrm << "Language: " << bl->toStringLanguage() << std::endl;
			std::cout << "Language: " << bl->toStringLanguage() << std::endl;
		}
		else
		{
			fstrm << "Language: " << bl->toStringLang(ep) << std::endl;
			std::cout << "Language: " << bl->toStringLang(ep) << std::endl;
		}
		fstrm << "Research items are: ";
		std::cout << "Research items are: ";
		for (auto& e : items)
		{
			fstrm << e.first << "; ";
			std::cout << e.first << "; ";
		}
		fstrm << std::endl;
		std::cout << std::endl;
		//
		fstrm << std::endl; std::cout << std::endl;
		fstrm << "Time examples:" << std::endl;
		std::cout << "Time examples:" << std::endl;
		fstrm << "time: get oprefix/suffix Language";
		std::cout << "time: get oprefix/suffix Language";
		beg = clock();
		Language* _fix_bl = choosePrefSuffLang(bl, option);
		fin = clock();
		std::cout << "(" << _fix_bl->getCount() << " words)" << " = " << fin - beg << std::endl;
		fstrm << "(" << _fix_bl->getCount() << " words)" << " = " << fin - beg << std::endl;
		std::cout << "time: get all sublanguages";
		fstrm << "time: get all sublanguages";
		beg = clock();
		auto sublanFIXBL_pair = getAllSublang(_fix_bl);
		fin = clock();
		std::cout << "(" << sublanFIXBL_pair.second.size() << " languages)" << " = " << fin - beg << std::endl;
		fstrm << "(" << sublanFIXBL_pair.second.size() << " languages)" << " = " << fin - beg << std::endl;
		std::vector<Language*> sublanFIXBL = sublanFIXBL_pair.second;
		int tableSize = sublanFIXBL.size();
		std::cout << "time: example for one semigroup operation";
		fstrm << "time: example for one semigroup operation";
		beg = clock();
		Language* afterOperation = operationSemigroup_F(bl,
			sublanFIXBL[tableSize - 1], sublanFIXBL[tableSize - 1], option);
		fin = clock();
		std::cout << "(language:" << sublanFIXBL[tableSize - 1]->toStringLang(ep) << " )"
			<< " = " << fin - beg << std::endl;
		fstrm << "(language:" << sublanFIXBL[tableSize - 1]->toStringLang(ep) << " )"
			<< " = " << fin - beg << std::endl;

		// build pri table of base language
		fstrm << std::endl; std::cout << std::endl;
		std::cout << "Building table PRI(B)..." << std::endl;
		fstrm << "Building table PRI(B)...";
		beg = clock();
		std::string tmpstr = tablePRI_aSsOPBL_opsg_toFile(bl, testname + "_tablePRI.csv", option, len, ep);
		fin = clock();
		ret[tmpstr] = std::make_pair(1, nullptr);
		fstrm << "done" << std::endl;
		fstrm << "timer: " << fin - beg << std::endl;
		fstrm << "table file name: " << tmpstr << std::endl;
		std::cout << "done" << std::endl;
		std::cout << "timer: " << fin - beg << std::endl;
		std::cout << "table file name: " << tmpstr << std::endl;
		// well, need reading table out from file
		std::cout << "Reading table from file...";
		typePRI_1 table = readTable_opsg_fromFile(tmpstr, bl, option);
		std::cout << "done" << std::endl;

		// check closure
		fstrm << std::endl; std::cout << std::endl;
		std::cout << "check closure..." << std::endl;
		fstrm << "check closure...";
		beg = clock();
		std::vector<int*> testCl = testClosure_pri1(table);
		fin = clock();
		int* clt = new int[testCl.size() * 2];
		for (int i = 0; i < testCl.size(); i++)
		{
			clt[i * 2 + 0] = testCl[i][0];
			clt[i * 2 + 1] = testCl[i][1];
		}
		ret["closure"] = std::make_pair(testCl.size(), clt);
		fstrm << "timer: " << fin - beg << std::endl;
		std::cout << "done" << std::endl;
		std::cout << "timer: " << fin - beg << std::endl;
		if (testCl.size())
		{
			fstrm << "NOT CLOSED! Result of operation on these pairs out of boundary: ";
			std::cout << "NOT CLOSED! Result of operation on these pairs out of boundary: ";
			for (auto e : testCl)
			{
				fstrm << '(' << e[0] << ',' << e[1] << "); ";
				std::cout << '(' << e[0] << ',' << e[1] << "); ";
			}
			fstrm << std::endl;
			std::cout << std::endl;
			//
			fstrm << "\n***Research halted.***\n";
			fstrm.close();
			std::cout << "\n***Research halted.***\n";
			return ret;
		}
		else
		{
			fstrm << "CLOSED, got magma.";
			std::cout << "CLOSED, got magma.";
			fstrm << std::endl;
			std::cout << std::endl;
		}

		// check associativity
		fstrm << std::endl; std::cout << std::endl;
		std::cout << "check associativity..." << std::endl;
		fstrm << "check associativity...";
		beg = clock();
		std::vector<int*> testAs = testAssociativity_pri1(table);
		fin = clock();
		int* ast = new int[testAs.size() * 3];
		for (int i = 0; i < testAs.size(); i++)
		{
			ast[i * 2 + 0] = testAs[i][0];
			ast[i * 2 + 1] = testAs[i][1];
			ast[i * 2 + 2] = testAs[i][2];
		}
		ret["associativity"] = std::make_pair(testAs.size(), ast);
		fstrm << "done" << std::endl;
		fstrm << "timer: " << fin - beg << std::endl;
		std::cout << "done" << std::endl;
		std::cout << "timer: " << fin - beg << std::endl;
		if (testAs.size())
		{
			fstrm << "NOT ASSOCIATIVITY! Operation on these triples not associative: ";
			std::cout << "NOT ASSOCIATIVITY! Operation on these triples not associative: ";
			for (auto e : testAs)
			{
				fstrm << '(' << e[0] << ',' << e[1] << ',' << e[2] << "); ";
				std::cout << '(' << e[0] << ',' << e[1] << ',' << e[2] << "); ";
			}
			fstrm << std::endl;
			std::cout << std::endl;
			//
			fstrm << "\n***Research halted.***\n";
			fstrm.close();
			std::cout << "\n***Research halted.***\n";
			return ret;
		}
		else
		{
			fstrm << "ASSOCIATIVE, got semigroup.";
			std::cout << "ASSOCIATIVE, got semigroup.";
			fstrm << std::endl;
			std::cout << std::endl;
		}

		for (std::pair<std::string, table_trans> item : items)
		{
			if (item.first.find("dempotent") != std::string::npos)
			{
				// find idempotents
				fstrm << std::endl; std::cout << std::endl;
				std::cout << "find idempotents..." << std::endl;
				fstrm << "find idempotents...";
				beg = clock();
				std::vector<int> idem = getIdempotent_pri1(table.second);
				fin = clock();
				ret["idempotent"] = std::make_pair(idem.size(), idem.data());
				fstrm << "done" << std::endl;
				fstrm << "timer: " << fin - beg << std::endl;
				std::cout << "done" << std::endl;
				std::cout << "timer: " << fin - beg << std::endl;
				if (idem.size())
				{
					fstrm << "GOT IDEMPOTENTS! In semigroup " << idem.size() << " idempotents: ";
					std::cout << "GOT IDEMPOTENTS! In semigroup " << idem.size() << " idempotents: ";
					for (int e : idem)
					{
						fstrm << e << "; ";
						std::cout << e << "; ";
					}
					fstrm << std::endl;
					std::cout << std::endl;
				}
				else
				{
					fstrm << "NOT IDEMPOTENT.";
					std::cout << "NOT IDEMPOTENT.";
					fstrm << std::endl;
					std::cout << std::endl;
				}
			}
			else if (item.first.find("dentity") != std::string::npos)
			{
				if ((item.first.find("wo") != std::string::npos)
					|| (item.first.find("oth") != std::string::npos))
				{
					// find two-side identity
					fstrm << std::endl; std::cout << std::endl;
					std::cout << "find two-side identity..." << std::endl;
					fstrm << "find two-side identity...";
					beg = clock();
					std::vector<int> tsi = getTSIdentity_pri1(table.second);
					fin = clock();
					ret["TSIdentity"] = std::make_pair(tsi.size(), tsi.data());
					fstrm << "done" << std::endl;
					fstrm << "timer: " << fin - beg << std::endl;
					std::cout << "done" << std::endl;
					std::cout << "timer: " << fin - beg << std::endl;
					if (tsi.size())
					{
						fstrm << "GOT TWO-SIDE IDENTITY! In semigroup: ";
						std::cout << "GOT TWO-SIDE IDENTITY! In semigroup: ";
						for (int e : tsi)
						{
							fstrm << e << "; ";
							std::cout << e << "; ";
						}
						fstrm << std::endl;
						std::cout << std::endl;
					}
					else
					{
						fstrm << "NOT TWO-SIDE IDENTITY.";
						std::cout << "NOT TWO-SIDE IDENTITY.";
						fstrm << std::endl;
						std::cout << std::endl;
					}
				}
				else if ((item.first.find("eft") != std::string::npos)
					|| (item.first.find("L") != std::string::npos)
					|| (item.first.find("l") != std::string::npos))
				{
					// find left identity
					fstrm << std::endl; std::cout << std::endl;
					std::cout << "find left identity..." << std::endl;
					fstrm << "find left identity...";
					beg = clock();
					std::vector<int> li = getLeftIdentity_pri1(table.second);
					fin = clock();
					ret["LeftIdentity"] = std::make_pair(li.size(), li.data());
					fstrm << "done" << std::endl;
					fstrm << "timer: " << fin - beg << std::endl;
					std::cout << "done" << std::endl;
					std::cout << "timer: " << fin - beg << std::endl;
					if (li.size())
					{
						fstrm << "GOT LEFT IDENTITY! In semigroup: ";
						std::cout << "GOT LEFT IDENTITY! In semigroup: ";
						for (int e : li)
						{
							fstrm << e << "; ";
							std::cout << e << "; ";
						}
						fstrm << std::endl;
						std::cout << std::endl;
					}
					else
					{
						fstrm << "NOT LEFT IDENTITY.";
						std::cout << "NOT LEFT IDENTITY.";
						fstrm << std::endl;
						std::cout << std::endl;
					}
				}
				else if ((item.first.find("ight") != std::string::npos)
					|| (item.first.find("R") != std::string::npos)
					|| (item.first.find("r") != std::string::npos))
				{
					// find right identity
					fstrm << std::endl; std::cout << std::endl;
					std::cout << "find right identity..." << std::endl;
					fstrm << "find right identity...";
					beg = clock();
					std::vector<int> ri = getRightIdentity_pri1(table.second);
					fin = clock();
					ret["RightIdentity"] = std::make_pair(ri.size(), ri.data());
					fstrm << "done" << std::endl;
					fstrm << "timer: " << fin - beg << std::endl;
					std::cout << "done" << std::endl;
					std::cout << "timer: " << fin - beg << std::endl;
					if (ri.size())
					{
						fstrm << "GOT RIGHT IDENTITY! In semigroup: ";
						std::cout << "GOT RIGHT IDENTITY! In semigroup: ";
						for (int e : ri)
						{
							fstrm << e << "; ";
							std::cout << e << "; ";
						}
						fstrm << std::endl;
						std::cout << std::endl;
					}
					else
					{
						fstrm << "NOT RIGHT IDENTITY.";
						std::cout << "NOT RIGHT IDENTITY.";
						fstrm << std::endl;
						std::cout << std::endl;
					}
				}
				else
				{
					fstrm << "WHAT KIND OF IDENTITY?";
					std::cout << "WHAT KIND OF IDENTITY?";
					fstrm << std::endl;
					std::cout << std::endl;
				}
			}
			else if ((item.first.find("eries") != std::string::npos)
				&& (item.first.find("ower") != std::string::npos))
			{
				// calculate power series
				fstrm << std::endl; std::cout << std::endl;
				std::cout << "calculate power series..." << std::endl;
				fstrm << "calculate power series...";
				beg = clock();
				std::map<int, std::pair<std::pair<int, int>,
					std::vector<int>>> ps = getPowerSeries_pri1(table.second);
				fin = clock();
				fstrm << "done" << std::endl;
				fstrm << "timer: " << fin - beg << std::endl;
				std::cout << "done" << std::endl;
				std::cout << "timer: " << fin - beg << std::endl;
				int tmpsize = 0;
				for (std::pair<int, std::pair<std::pair<int, int>,
					std::vector<int>>> e : ps)
				{
					if (e.second.first.first + e.second.first.second > tmpsize)
						tmpsize = e.second.first.first + e.second.first.second;
				}
				tmpsize += 2;
				int* pst = new int[table.second.first * tmpsize]();
				fstrm << "(index, period) power series in semigroup: " << std::endl;
				std::cout << "(index, period) power series in semigroup: " << std::endl;
				for (int i = 0; i < table.second.first; i++)
				{
					fstrm << "element " << i << " ";
					std::cout << "element " << i << " ";
					fstrm << "(" << ps[i].first.first << "," << ps[i].first.second << ") : ";
					std::cout << "(" << ps[i].first.first << "," << ps[i].first.second << ") : ";
					int j = 0;
					pst[i * tmpsize + j] = ps[i].first.first;
					j += 1;
					pst[i * tmpsize + j] = ps[i].first.second;
					j += 1;
					for (int e : ps[i].second)
					{
						fstrm << e << "; ";
						std::cout << e << "; ";
						pst[i * tmpsize + j] = e;
						j += 1;
					}
					fstrm << std::endl;
					std::cout << std::endl;
				}
				ret["PowerSeries"] = std::make_pair(table.second.first, pst);
			}
			else if (item.first.find("ommutat") != std::string::npos)
			{
				// get commutativity
				fstrm << std::endl; std::cout << std::endl;
				std::cout << "get commutativity..." << std::endl;
				fstrm << "get commutativity...";
				beg = clock();
				std::pair<int, std::vector<int*>> comm = haveCommutativity_pri1(table.second);
				fin = clock();
				fstrm << "done" << std::endl;
				fstrm << "timer: " << fin - beg << std::endl;
				std::cout << "done" << std::endl;
				std::cout << "timer: " << fin - beg << std::endl;
				int* ct = new int[comm.second.size() * 2 + 1]();
				ct[0] = comm.first;
				if (comm.first == comm.second.size())
				{
					fstrm << "GOT COMMUTATIVE SEMIGROUP!";
					std::cout << "GOT COMMUTATIVE SEMIGROUP!";
					fstrm << std::endl;
					std::cout << std::endl;
				}
				fstrm << comm.second.size() << " pairs are commutative: ";
				std::cout << comm.second.size() << " pairs are commutative: ";
				int i = 0;
				for (int* e : comm.second)
				{
					fstrm << "(" << e[0] << "," << e[1] << "); ";
					std::cout << "(" << e[0] << "," << e[1] << "); ";
					ct[1 + i * 2 + 0] = e[0];
					ct[1 + i * 2 + 1] = e[1];
				}
				fstrm << std::endl;
				std::cout << std::endl;
				ret["commutativity"] = std::make_pair(comm.second.size(), ct);
			}
			else if ((item.first.find("in") != std::string::npos)
				&& (item.first.find("ub") != std::string::npos)
				&& (item.first.find("emi") != std::string::npos)
				&& (item.first.find("roup") != std::string::npos))
			{
				// calculate generated min sub semi group
				fstrm << std::endl; std::cout << std::endl;
				std::cout << "calculate generated minsubsemigroup..." << std::endl;
				fstrm << "calculate generated minsubsemigroup...";
				beg = clock();
				table_trans gmssg = minSubSemiGroup_generated(table.second, item.second);
				fin = clock();
				fstrm << "done" << std::endl;
				fstrm << "timer: " << fin - beg << std::endl;
				std::cout << "done" << std::endl;
				std::cout << "timer: " << fin - beg << std::endl;
				ret["MinSubSemiGroup"] = gmssg;
				fstrm << "By {";
				std::cout << "By {";
				for (int i = 0; i < item.second.first; i++)
				{
					if (item.second.second[i])
					{
						fstrm << i << ",";
						std::cout << i << ",";
					}
				}
				fstrm << "} generated: {";
				std::cout << "} generated: {";
				for (int i = 0; i < gmssg.first; i++)
				{
					if (gmssg.second[i])
					{
						fstrm << i << ",";
						std::cout << i << ",";
					}
				}
				fstrm << "}" << std::endl;
				std::cout << "}" << std::endl;
			}
			else if ((item.first.find("inary") != std::string::npos)
				&& (item.first.find("elation") != std::string::npos)
				&& (item.first.find("hain") == std::string::npos))
			{
				// calculate binary relation table
				fstrm << std::endl; std::cout << std::endl;
				std::cout << "calculate binary relation table..." << std::endl;
				fstrm << "calculate binary relation table...";
				beg = clock();
				std::string brf = binaryRelation_table(table.second, testname + "_binaryRelationTable.csv", 'p');
				fin = clock();
				fstrm << "done" << std::endl;
				fstrm << "timer: " << fin - beg << std::endl;
				std::cout << "done" << std::endl;
				std::cout << "timer: " << fin - beg << std::endl;
				ret["BinaryRelation"] = std::make_pair(1, nullptr);
				fstrm << "binary relation table file name: " << brf << std::endl;
				std::cout << "binary relation table file name: " << brf << std::endl;
			}
			else if ((item.first.find("inary") != std::string::npos)
				&& (item.first.find("elation") != std::string::npos)
				&& (item.first.find("hain") != std::string::npos))
			{
				// calculate binary relation chain
				fstrm << std::endl; std::cout << std::endl;
				std::cout << "calculate binary relation chain..." << std::endl;
				fstrm << "calculate binary relation chain...";
				//std::pair<std::vector<int*>, bool*> brt = readBRTable_fromFile(table.second, brf); // no need
				beg = clock();
				std::vector<int*> brc = binaryRelation_2s2iChain(table.second,
					item.second.second[0], item.second.second[1], item.second.first);
				fin = clock();
				fstrm << "done" << std::endl;
				fstrm << "timer: " << fin - beg << std::endl;
				std::cout << "done" << std::endl;
				std::cout << "timer: " << fin - beg << std::endl;
				int* brct = new int[brc.size() * 2];
				fstrm << "Chain for {("
					<< item.second.second[0]
					<< ","
					<< item.second.second[1]
					<< "),"
					<< item.second.first
					<< "} is : ";
				std::cout << "Chain for {("
					<< item.second.second[0] << "," << item.second.second[1] << "),"
					<< item.second.first
					<< "} is : ";
				for (int i = 0; i < brc.size(); i++)
				{
					if (brc[i] != nullptr)
					{
						brct[i * 2 + 0] = brc[i][0];
						brct[i * 2 + 1] = brc[i][1];
						fstrm << "(" << brc[i][0] << "," << brc[i][1] << ");";
						std::cout << "(" << brc[i][0] << "," << brc[i][1] << ");";
					}
					else
					{
						brct[i * 2 + 0] = -2;
						brct[i * 2 + 1] = -2;
						fstrm << "(BAD STATE F);";
						std::cout << "(BAD STATE F);";
					}
				}
				ret["BinaryRelationChain"] = std::make_pair(brc.size(), brct);
				fstrm << std::endl;
				std::cout << std::endl;
			}
			else
			{
				fstrm << std::endl; std::cout << std::endl;
				fstrm << "?WHAT IS ITEM: " << item.first << std::endl;
				std::cout << "?WHAT IS ITEM: " << item.first << std::endl;
			}
		}
		// finish
		fstrm << "\n***Research finished.***\n";
		fstrm.close();
		std::cout << "\n***Research finished.***\n";
		return ret;
	}
	void research_example()
	{// provide an example of providing researching items
		Language* b = readLangFromString("{a,aaa}");
		std::map<std::string, table_trans> items;
		//items["TablePRI"] = std::make_pair(1, nullptr); // no need, process will automatically do. 
		//items["closure"] = std::make_pair(1, nullptr); // no need, process will automatically do. 
		//items["associativity"] = std::make_pair(1, nullptr); // no need, process will automatically do. 
		items["Commutativity"] = std::make_pair(1, nullptr);
		// => <how many pairs, 1(how many are tested)+pair-num*2>
		items["Idempotents"] = std::make_pair(1, nullptr);
		// => <idem-num, idem-num*1>
		items["TwoSideIdentity"] = std::make_pair(1, nullptr);
		items["LeftIdentity"] = std::make_pair(1, nullptr);
		items["RightIdentity"] = std::make_pair(1, nullptr);
		// => <ident-num, ident-num*1>
		items["PowerSerieses"] = std::make_pair(1, nullptr);
		// => <elem-num, (though it is a square table but can be taken as) elem-num*(2+ind+per)>
		items["MinSubSemiGroup{0,1}"] = std::make_pair(2, new int[2] {1, 1}); // good, whole list also good
		// => <elem-num, is_in-boolValueList>
		items["subGroupsSemigroup"] = std::make_pair(1, nullptr);
		items["BinaryRelationship"] = std::make_pair(1, nullptr);
		// => <1, nullptr>
		items["BinaryRelationshipChains"] = std::make_pair(2, new int[2] {0, 3}); // <which way, {p1,p2}>
		// => <chain-length,chain-length*2>
		items["Breakfest"] = std::make_pair(1, nullptr); // undefined item just return nothing
		// to be continued ...
		std::map<std::string, table_trans> res_res = intergrated_research_process(
			b, "reasearch_example_aaa", items, "BfollowA", 'l', "_");
		return;
	}
	void research_example(Language* b, std::string fn)
	{// provide an example of providing researching items
		//Language* b; // provided
		std::map<std::string, table_trans> items;
		//items["TablePRI"] = std::make_pair(1, nullptr); // no need, process will automatically do. 
		//items["closure"] = std::make_pair(1, nullptr); // no need, process will automatically do. 
		//items["associativity"] = std::make_pair(1, nullptr); // no need, process will automatically do. 
		items["Commutativity"] = std::make_pair(1, nullptr);
		// =:: <how many pairs, 1(how many are tested)+pair-num*2>
		items["Idempotents"] = std::make_pair(1, nullptr);
		// =:: <idem-num, idem-num*1>
		items["TwoSideIdentity"] = std::make_pair(1, nullptr);
		items["LeftIdentity"] = std::make_pair(1, nullptr);
		items["RightIdentity"] = std::make_pair(1, nullptr);
		// =:: <ident-num, ident-num*1>
		items["PowerSerieses"] = std::make_pair(1, nullptr);
		// =:: <elem-num, (though it is a square table but can be taken as) elem-num*(2+ind+per)>
		items["MinSubSemiGroup{0,1}"] = std::make_pair(2, new int[2] {1, 1}); // good, whole hash list also good
		// =:: <elem-num, is_in-boolValueHashList>
		items["BinaryRelationship"] = std::make_pair(1, nullptr);
		// =:: <1, nullptr>
		items["BinaryRelationshipChains"] = std::make_pair(2, new int[2] {0, 3}); // <which way, {p1,p2}>
		// =:: <chain-length,chain-length*2>
		items["Breakfest"] = std::make_pair(1, nullptr); // undefined item just return nothing
		// to be continued ...
		std::map<std::string, table_trans> res_res = intergrated_research_process(
			b, fn, items, "BfollowA", 'l', "_");
		return;
	}

	// non-deterministic version
	// table of non-deterministic automaton NSPRI is "int**": 
	// one star "*" for table itself: it's a table of "int*"
	// its element "int*" is an array: 1st element for amount of elements in this array, then its elements
	std::pair<
		std::pair<std::vector<Language*>, std::vector<Language*>>,
		int**> getTableNSPRI_twoLang(
			Language* la, Language* lb, std::string option, bool onlyAttainable)
	{// make table of nondeterm. automaton NSPRI
		la->cleanBadWords(false);
		lb->cleanBadWords(false);
		Language* asl = choosePrefSuffLang(lb, option);
		asl->deleteWordByString("");
		std::vector<Language*> stt; // to represent null set, use Language
		std::vector<Language*> newLang;
		Language* nullLang = new Language(lb->getAlphabet());
		stt.push_back(nullLang);
		Language* beginLang = new Language(lb->getAlphabet());
		beginLang->addWord_("");
		beginLang->cleanBadWords(false);
		stt.push_back(beginLang);
		newLang.push_back(beginLang);
		std::vector<Language*> AbyWord;
		for (int i = 1; i <= la->getCount(); i++)
		{
			Language* tmp = new Language(la->getAlphabet());
			tmp->addWord_(la->getWord_afterCleaning(i));
			tmp->cleanBadWords(false);
			AbyWord.push_back(tmp);
		}
		std::vector<int**> tablePRI;
		int** tmp = new int* [AbyWord.size()];
		for (int i = 0; i < AbyWord.size(); i++)
		{ tmp[i] = new int[2] {1, 0}; }
		tablePRI.push_back(tmp);
		std::vector<Language*>::iterator it = newLang.begin();
		while (it != newLang.end())
		{
			tmp = new int* [AbyWord.size()];
			for (int i = 0; i < AbyWord.size(); i++)
			{
				Language* tmpL = Phi_AminusB(AbyWord[i], lb, *it, option);
				tmpL->cleanBadWords(false);
				if (tmpL->getCount() == 0)
				{ tmp[i] = new int[2] {1, 0}; }
				else
				{
					tmp[i] = new int[1 + tmpL->getCount()]();
					tmp[i][0] = tmpL->getCount();
					for (int j = 0; j < tmpL->getCount(); j++)
					{
						Language* tmpWL = new Language(tmpL->getAlphabet());
						tmpWL->addWord(tmpL->getWord_afterCleaning(j + 1), false);
						int findflag = 0;
						for (; findflag < stt.size(); findflag++)
						{ if (stt[findflag]->compareLanguage(tmpWL))
							{ tmp[i][1 + j] = findflag; break; } }
						if (findflag == stt.size())
						{
							asl->deleteWordByString(tmpL->getWord_afterCleaning(j + 1));
							stt.push_back(tmpWL);
							newLang.push_back(tmpWL);
							it = newLang.begin();
							for (; findflag < stt.size(); findflag++)
							{ if (stt[findflag]->compareLanguage(tmpWL))
								{ tmp[i][1 + j] = findflag; break; } }
						}
					}
				}
			}
			tablePRI.push_back(tmp);
			it = newLang.erase(it);
		}
		if (!onlyAttainable)
		{
			for (int i = 0; i < asl->getCount(); i++)
			{
				Language* tmpWL = new Language(asl->getAlphabet());
				tmpWL->addWord(asl->getWord_afterCleaning(i + 1), false);
				newLang.push_back(tmpWL);
				stt.push_back(tmpWL);
			}
			it = newLang.begin();
			while (newLang.size())
			{
				tmp = new int* [AbyWord.size()];
				for (int i = 0; i < AbyWord.size(); i++)
				{
					Language* tmpL = Phi_AminusB(AbyWord[i], lb, *it, option);
					tmpL->cleanBadWords(false);
					if (tmpL->getCount() == 0)
					{ tmp[i] = new int[2] {1, 0}; }
					else
					{
						tmp[i] = new int[1 + tmpL->getCount()];
						tmp[i][0] = tmpL->getCount();
						for (int k = 0; k < tmpL->getCount(); k++)
						{
							Language* tmpWL = new Language(tmpL->getAlphabet());
							tmpWL->addWord(tmpL->getWord_afterCleaning(k + 1), false);
							for (int j = 0; j < stt.size(); j++)
							{ if (stt[j]->compareLanguage(tmpWL))
								{ tmp[i][1 + k] = j; break; } }
						}
					}
				}
				tablePRI.push_back(tmp);
				it = newLang.erase(it);
			}
		}
		int** tablePRI_res = new int* [tablePRI.size() * AbyWord.size()];
		for (int i = 0; i < tablePRI.size(); i++)
		{ for (int j = 0; j < AbyWord.size(); j++)
			{ tablePRI_res[i * AbyWord.size() + j] = tablePRI[i][j]; } }
		return std::make_pair(std::make_pair(stt, AbyWord), tablePRI_res);
	}
	void getTableNSPRI_printProgram(
		std::pair<std::pair<std::vector<Language*>, 
		std::vector<Language*>>, int**> test,
		char len, std::string ep)
	{// print table of NSPRI on console
		std::string tem = "0|";
		int w1 = 0, w2 = 0, teml = tem.length();
		if (len == 'l')
		{ for (auto& e : test.first.first) 
			if (e->toStringLanguage().length() > w1) 
				w1 = e->toStringLanguage().length(); }
		else
		{ for (auto& e : test.first.first) 
			if (e->toStringLang(ep).length() > w1) 
				w1 = e->toStringLang(ep).length(); }
		if (len == 'l')
		{ for (auto& e : test.first.second) 
			if (e->toStringLanguage().length() > w2) 
				w2 = e->toStringLanguage().length(); }
		else
		{ for (auto& e : test.first.second) 
			if (e->toStringLang(ep).length() > w2) 
				w2 = e->toStringLang(ep).length(); }
		for (int i = 0; i < test.first.first.size(); i++)
			for (int j = 0; j < test.first.second.size(); j++)
				if (test.second[i * test.first.second.size() + j][0] * teml > w2)
					w2 = test.second[i * test.first.second.size() + j][0] * teml;
		std::cout << std::left << std::setw(w1) << "";
		if (len == 'l')
		{ for (auto& e : test.first.second) 
			std::cout << std::left << std::setw(w2) 
			<< e->toStringLanguage(); }
		else
		{ for (auto& e : test.first.second) 
			std::cout << std::left << std::setw(w2) 
			<< e->toStringLang(ep); }
		std::cout << std::endl;
		for (int i = 0; i < test.first.first.size(); i++)
		{
			if (len == 'l')
			{ std::cout << std::left << std::setw(w1) 
				<< test.first.first[i]->toStringLanguage(); }
			else
			{ std::cout << std::left << std::setw(w1) 
				<< test.first.first[i]->toStringLang(ep); }
			for (int j = 0; j < test.first.second.size(); j++)
			{
				tem = std::to_string(test.second[
					i * test.first.second.size() + j][1]);
				for (int k = 1; 
					k < test.second[i * test.first.second.size() + j][0]; k++)
				{ tem += "|" + std::to_string(test.second[
					i * test.first.second.size() + j][1 + k]); }
				std::cout << std::left << std::setw(w2) << tem;
			}
			std::cout << std::endl;
		}
	}
	std::pair<std::vector<Language*>, int**> getTableNSPRI_oneLang(
		Language* lb, std::string option)
	{// make table of NSPRI for 1 language
		lb->cleanBadWords(false);
		Language* asl = choosePrefSuffLang(lb, option);
		std::vector<Language*> langlist;
		Language* nullLang = new Language(lb->getAlphabet());
		langlist.push_back(nullLang);
		for (int i = 1; i <= asl->getCount(); i++)
		{
			Language* tmp = new Language(asl->getAlphabet());
			tmp->addWord_(asl->getWord_afterCleaning(i));
			tmp->cleanBadWords(false);
			langlist.push_back(tmp);
		}
		int** tablePRI = new int* [langlist.size() * langlist.size()];
		for (int i = 0; i < langlist.size(); i++)
		{
			for (int j = 0; j < langlist.size(); j++)
			{
				Language* tmpL = operationSemigroup_F(lb, 
					langlist[i], langlist[j], option);
				tmpL->cleanBadWords(false);
				if (tmpL->getCount() == 0)
				{ tablePRI[i * langlist.size() + j] = new int[2] {1, 0}; }
				else
				{
					tablePRI[i * langlist.size() + j] = new int[1 + tmpL->getCount()]();
					tablePRI[i * langlist.size() + j][0] = tmpL->getCount();
					for (int k = 0; k < tmpL->getCount(); k++)
					{
						Language* tmpWL = new Language(tmpL->getAlphabet());
						tmpWL->addWord(tmpL->getWord_afterCleaning(k + 1), false);
						for (int m = 0; m < langlist.size(); m++)
						{ if (langlist[m]->compareLanguage(tmpWL))
							{ tablePRI[i * langlist.size() + j][1 + k] = m; break; } }
					}
				}
			}
		}
		return std::make_pair(langlist, tablePRI);
	}
	void getTableNSPRI_printProgram(
		std::pair<std::vector<Language*>, int**> test,
		char len, std::string ep)
	{// print table of groupoid of NSPRI on console
		std::string tem = "0|";
		int w1 = 0, w2 = 0, teml = tem.length();
		if (len == 'l') { for (auto& e : test.first) 
			if (e->toStringLanguage().length() > w1) 
				w1 = e->toStringLanguage().length(); }
		else { for (auto& e : test.first) 
			if (e->toStringLang(ep).length() > w1) 
				w1 = e->toStringLang(ep).length(); }
		w2 = w1;
		for (int i = 0; i < test.first.size(); i++)
			for (int j = 0; j < test.first.size(); j++)
				if (test.second[i * test.first.size() + j][0] * teml > w2)
					w2 = test.second[i * test.first.size() + j][0] * teml;
		std::cout << std::left << std::setw(w1) << "";
		if (len == 'l') { for (auto& e : test.first) 
			std::cout << std::left << std::setw(w2) 
			<< e->toStringLanguage(); }
		else { for (auto& e : test.first) 
			std::cout << std::left << std::setw(w2) 
			<< e->toStringLang(ep); }
		std::cout << std::endl;
		for (int i = 0; i < test.first.size(); i++)
		{ 
			if (len == 'l') { std::cout << std::left << std::setw(w1) 
				<< test.first[i]->toStringLanguage(); }
			else { std::cout << std::left << std::setw(w1) 
				<< test.first[i]->toStringLang(ep); }
			for (int j = 0; j < test.first.size(); j++)
			{
				tem = std::to_string(test.second[i * test.first.size() + j][1]);
				for (int k = 1; k < test.second[i * test.first.size() + j][0]; k++)
				{ tem += "|" + std::to_string(test.second[
					i * test.first.size() + j][1 + k]); }
				std::cout << std::left << std::setw(w2) << tem; }
			std::cout << std::endl;
		}
	}
	std::string tableNSPRI_toFile(
		std::pair<std::pair<std::vector<Language*>, 
		std::vector<Language*>>, int**> test,
		std::string filename, char len, std::string ep)
	{// save table of NSPRI in file
		std::fstream output_fstream;
		output_fstream.open(filename, std::ios_base::out);
		if (!output_fstream.is_open())
		{ std::cout << "tableNSPRI_toFile can't open file\n";
			return ""; }
		std::string tem;
		output_fstream << ";";
		if (len == 'l') { for (auto& e : test.first.second) 
			output_fstream << e->toStringLanguage() << ";"; }
		else { for (auto& e : test.first.second) 
			output_fstream << e->toStringLang(ep) << ";"; }
		output_fstream << std::endl;
		for (int i = 0; i < test.first.first.size(); i++)
		{
			if (len == 'l') { output_fstream 
				<< test.first.first[i]->toStringLanguage() << ";"; }
			else { output_fstream 
				<< test.first.first[i]->toStringLang(ep) << ";"; }
			for (int j = 0; j < test.first.second.size(); j++)
			{
				tem = std::to_string(test.second[i * test.first.second.size() + j][1]);
				for (int k = 1; k < test.second[i * test.first.second.size() + j][0]; k++)
				{ tem += "|" + std::to_string(test.second[
					i * test.first.second.size() + j][1 + k]); }
				output_fstream << tem << ";"; }
			output_fstream << std::endl;
		}
		output_fstream.close();
		return filename;
	}
	std::string tableNSPRI_toFile(std::pair<std::vector<Language*>, int**> test,
		std::string filename, char len, std::string ep)
	{// save table of groupoid of NSPRI in file
		std::fstream output_fstream;
		output_fstream.open(filename, std::ios_base::out);
		if (!output_fstream.is_open())
		{
			std::cout << "tableNSPRI_toFile can't open file\n";
			return "";
		}
		std::string tem;
		output_fstream << ";";
		if (len == 'l')
		{
			for (auto& e : test.first) output_fstream << e->toStringLanguage() << ";";
		}
		else
		{
			for (auto& e : test.first) output_fstream << e->toStringLang(ep) << ";";
		}
		output_fstream << std::endl;
		for (int i = 0; i < test.first.size(); i++)
		{
			if (len == 'l')
			{
				output_fstream << test.first[i]->toStringLanguage() << ";";
			}
			else
			{
				output_fstream << test.first[i]->toStringLang(ep) << ";";
			}
			for (int j = 0; j < test.first.size(); j++)
			{
				tem = std::to_string(test.second[i * test.first.size() + j][1]);
				for (int k = 1; k < test.second[i * test.first.size() + j][0]; k++)
				{
					tem += "|" + std::to_string(test.second[i * test.first.size() + j][1 + k]);
				}
				output_fstream << tem << ";";
			}
			output_fstream << std::endl;
		}
		output_fstream.close();
		return filename;
	}
	typeNSPRI_2 readTableNSPRI_fromFile(
		std::string filename, std::set<std::string> nw)
	{// read table of NSPRI from file
		// return < < languages-lines, languages-columns > , table >
		std::fstream fs;
		fs.open(filename, std::ios_base::in);
		if (!fs.is_open())
		{
			std::cout << 
				"readTableNSPRI_fromFile(file,null_words) can't open file\n";
			return std::make_pair(
				std::make_pair(std::vector<Language*>(), 
					std::vector<Language*>()), nullptr);
		}
		std::vector<Language*> langLine, langCol;
		std::vector<std::vector<int*>> tableSG;
		int j = 0, col = 0;
		std::string tmpLine;
		std::getline(fs, tmpLine);
		while (!tmpLine.empty())
		{
			if (tmpLine.substr(0, tmpLine.find_first_of(';')).empty())
			{
				tmpLine = tmpLine.substr(tmpLine.find_first_of(';') + 1);
				while (!tmpLine.empty())
				{
					std::string tmps = tmpLine.substr(0, tmpLine.find_first_of(';'));
					Language* tmpl = readLangFromString(tmps, nw);
					langCol.push_back(tmpl);
					tmpLine = tmpLine.substr(tmpLine.find_first_of(';') + 1);
				}
			}
			else
			{
				std::string tmps = tmpLine.substr(0, tmpLine.find_first_of(';'));
				Language* tmpl = readLangFromString(tmps, nw);
				langLine.push_back(tmpl);
				tmpLine = tmpLine.substr(tmpLine.find_first_of(';') + 1);
				std::vector<int*> tmpvector;
				j = 0;
				while (!tmpLine.empty())
				{
					int k = 0;
					std::string cell = tmpLine.substr(0, tmpLine.find_first_of(';'));
					for (char e : cell)if (e == '|') k += 1;
					int* tempcell = new int[1 + k + 1]();
					tempcell[0] = k + 1;
					k = 1;
					while ((!cell.empty()) && (k <= tempcell[0]))
					{
						tempcell[k] = (atoi(cell.substr(0, cell.find_first_of('|')).c_str()) == 0
							&&
							cell.substr(0, cell.find_first_of('|')) != "0"
							) ? (-1) : atoi(cell.substr(0, cell.find_first_of('|')).c_str());
						cell = cell.substr(cell.find_first_of('|') + 1);
						k += 1;
					}
					tmpvector.push_back(tempcell);
					tmpLine = tmpLine.substr(tmpLine.find_first_of(';') + 1);
					j += 1;
				}
				if (!col) { col = j; }
				else if (j > col)
				{ col = -1; // line length changed
					break; }
				tableSG.push_back(tmpvector);
			}
			std::getline(fs, tmpLine);
			if (tmpLine.empty()) break;
		}
		fs.close();
		if (col < 0 || col != langCol.size())
			return std::make_pair(make_pair(std::vector<Language*>(), 
				std::vector<Language*>()), nullptr);
		int** tableSG_res = new int* [langLine.size() * langCol.size()];
		col = 0;
		for (std::vector<int*> e : tableSG)
		{
			j = 0;
			for (int* ee : e)
			{ tableSG_res[col * langCol.size() + j] = ee;
				j += 1; }
			col += 1;
			e.clear();
		}
		tableSG.clear();
		return std::make_pair(make_pair(langLine, langCol), tableSG_res);
	}
	typePRI_2 getPRI_fromNSPRI(typeNSPRI_2 t)
	{// if non-determinism is not essential, 
		// then can get a table of PRI from table of NSPRI
		int* res = new int[t.first.first.size() * t.first.second.size()];
		for (int i = 0; i < t.first.first.size(); i++)
		{
			for (int j = 0; j < t.first.second.size(); j++)
			{
				if (t.second[i * t.first.second.size() + j][0] == 1)
				{ res[i * t.first.second.size() + j] 
						= t.second[i * t.first.second.size() + j][1]; }
				else
				{ delete[] res; res = nullptr;
					return std::make_pair(t.first, nullptr); }
			}
		}
		return std::make_pair(t.first, res);
	}
	pri1_table getPRI_fromNSPRI(typeNSPRI_1 t)
	{// get a table of PRI from table of NSPRI (groupoid ver.)
		int* res = new int[t.first.size() * t.first.size()];
		for (int i = 0; i < t.first.size(); i++)
		{
			for (int j = 0; j < t.first.size(); j++)
			{
				if (t.second[i * t.first.size() + j][0] == 1)
				{ res[i * t.first.size() + j] = t.second[i * t.first.size() + j][1]; }
				else
				{ delete[] res; res = nullptr;
					return std::make_pair(0, nullptr); }
			}
		}
		return std::make_pair(t.first.size(), res);
	}
	typePRI_1 getPRI_fromNSPRI(typeNSPRI_1 t, Language* b)
	{// if non-determinism is not essential, then can get a table of PRI from table of NSPRI (groupoid)
		// and as a feature of NSPRI, union of states is language itself
		int* res = new int[t.first.size() * t.first.size()];
		for (int i = 0; i < t.first.size(); i++)
		{
			for (int j = 0; j < t.first.size(); j++)
			{
				if (t.second[i * t.first.size() + j][0] == 1)
				{
					res[i * t.first.size() + j] = t.second[i * t.first.size() + j][1];
				}
				else
				{
					delete[] res;
					res = nullptr;
					return std::make_pair(std::make_pair(b, t.first), std::make_pair(0, nullptr));
				}
			}
		}
		Language* bb;
		if (b == nullptr)
		{
			bb = new Language(t.first[0]->getAlphabet());
			for (int i = 0; i < t.first.size(); i++)
			{
				if (t.first[i]->getCount() == 1)
					bb->addWord(t.first[i]->getWord_afterCleaning(1), false);
			}
		}
		else
		{
			bb = b;
		}
		return std::make_pair(std::make_pair(bb, t.first), std::make_pair(0, nullptr));
	}
	std::map<std::string, table_trans> intergrated_research_process_NS(
		Language* bl, std::string testname, std::map<std::string, table_trans> items,
		std::string option, char len, std::string ep)
	{// it's like version for deterministic version PRI
		clock_t beg, fin;
		std::fstream fstrm;
		fstrm.open(testname + "_NSPRI_researchOnSemigroup_mainInfo.txt", std::ios_base::out);
		if (!fstrm.is_open())
		{
			std::cout << "intergrated_research_process: can't open file _NSPRI_researchOnSemigroup\n";
			return std::map<std::string, table_trans>();
		}
		std::map<std::string, table_trans> ret;

		// introduction
		if (len == 'l')
		{
			fstrm << "Language: " << bl->toStringLanguage() << std::endl;
			std::cout << "Language: " << bl->toStringLanguage() << std::endl;
		}
		else
		{
			fstrm << "Language: " << bl->toStringLang(ep) << std::endl;
			std::cout << "Language: " << bl->toStringLang(ep) << std::endl;
		}
		fstrm << "Research items are: ";
		std::cout << "Research items are: ";
		for (auto& e : items)
		{
			fstrm << e.first << "; ";
			std::cout << e.first << "; ";
		}
		fstrm << std::endl;
		std::cout << std::endl;
		//
		fstrm << std::endl; std::cout << std::endl;
		fstrm << "Time examples:" << std::endl;
		std::cout << "Time examples:" << std::endl;
		fstrm << "time: get oprefix/suffix Language";
		std::cout << "time: get oprefix/suffix Language";
		beg = clock();
		Language* _fix_bl = choosePrefSuffLang(bl, option);
		fin = clock();
		std::cout << "(" << _fix_bl->getCount() << " words)" << " = " << fin - beg << std::endl;
		fstrm << "(" << _fix_bl->getCount() << " words)" << " = " << fin - beg << std::endl;
		std::cout << "time: get all sublanguages";
		fstrm << "time: get all sublanguages";
		beg = clock();
		auto sublanFIXBL_pair = getAllSublang(_fix_bl);
		fin = clock();
		std::cout << "(" << sublanFIXBL_pair.second.size() << " languages)" << " = " << fin - beg << std::endl;
		fstrm << "(" << sublanFIXBL_pair.second.size() << " languages)" << " = " << fin - beg << std::endl;
		std::vector<Language*> sublanFIXBL = sublanFIXBL_pair.second;
		int tableSize = sublanFIXBL.size();
		std::cout << "time: example for one semigroup operation";
		fstrm << "time: example for one semigroup operation";
		beg = clock();
		Language* afterOperation = operationSemigroup_F(bl,
			sublanFIXBL[tableSize - 1], sublanFIXBL[tableSize - 1], option);
		fin = clock();
		std::cout << "(language:" << sublanFIXBL[tableSize - 1]->toStringLang(ep) << " )"
			<< " = " << fin - beg << std::endl;
		fstrm << "(language:" << sublanFIXBL[tableSize - 1]->toStringLang(ep) << " )"
			<< " = " << fin - beg << std::endl;

		// build nspri table of base language
		fstrm << std::endl; std::cout << std::endl;
		std::cout << "Building table NSPRI(B)..." << std::endl;
		fstrm << "Building table NSPRI(B)...";
		beg = clock();
		std::string tmpstr = tableNSPRI_toFile(
			getTableNSPRI_oneLang(bl, option), testname + "_tableNSPRI.csv", len, ep);
		fin = clock();
		ret[tmpstr] = std::make_pair(1, nullptr);
		fstrm << "done" << std::endl;
		fstrm << "timer: " << fin - beg << std::endl;
		fstrm << "table file name: " << tmpstr << std::endl;
		std::cout << "done" << std::endl;
		std::cout << "timer: " << fin - beg << std::endl;
		std::cout << "table file name: " << tmpstr << std::endl;
		// well, need reading table out from file
		std::cout << "Reading table from file...";
		typeNSPRI_2 ns2_table = readTableNSPRI_fromFile(tmpstr, { ep });
		typeNSPRI_1 ns1_table = std::make_pair(ns2_table.first.first, ns2_table.second);
		std::cout << "done" << std::endl;

		// check nspri table to pri table
		fstrm << std::endl; std::cout << std::endl;
		std::cout << "get PRI table from NSPRI(B)..." << std::endl;
		fstrm << "get PRI table from NSPRI(B)..." << std::endl;
		beg = clock();
		pri1_table table = getPRI_fromNSPRI(ns1_table);
		fin = clock();
		ret["PRI_NSPRI"] = table;
		fstrm << "done" << std::endl;
		fstrm << "timer: " << fin - beg << std::endl;
		std::cout << "done" << std::endl;
		std::cout << "timer: " << fin - beg << std::endl;
		if (!table.first)
		{
			fstrm << "ESSENTIALLY NONDETERMINATED!";
			std::cout << "ESSENTIALLY NONDETERMINATED!";
			fstrm << std::endl;
			std::cout << std::endl;
			//
			fstrm << "\n***Research halted.***\n";
			fstrm.close();
			std::cout << "\n***Research halted.***\n";
			return ret;
		}
		else
		{
			fstrm << "Got PRI from NSPRI.";
			std::cout << "Got PRI from NSPRI.";
			fstrm << std::endl;
			std::cout << std::endl;
		}

		// check closure
		fstrm << std::endl; std::cout << std::endl;
		std::cout << "check closure..." << std::endl;
		fstrm << "check closure...";
		beg = clock();
		std::vector<int*> testCl = testClosure_pri1(table);
		fin = clock();
		int* clt = new int[testCl.size() * 2];
		for (int i = 0; i < testCl.size(); i++)
		{
			clt[i * 2 + 0] = testCl[i][0];
			clt[i * 2 + 1] = testCl[i][1];
		}
		ret["closure"] = std::make_pair(testCl.size(), clt);
		fstrm << "timer: " << fin - beg << std::endl;
		std::cout << "done" << std::endl;
		std::cout << "timer: " << fin - beg << std::endl;
		if (testCl.size())
		{
			fstrm << "NOT CLOSED! Result of operation on these pairs out of boundary: ";
			std::cout << "NOT CLOSED! Result of operation on these pairs out of boundary: ";
			for (auto e : testCl)
			{
				fstrm << '(' << e[0] << ',' << e[1] << "); ";
				std::cout << '(' << e[0] << ',' << e[1] << "); ";
			}
			fstrm << std::endl;
			std::cout << std::endl;
			//
			fstrm << "\n***Research halted.***\n";
			fstrm.close();
			std::cout << "\n***Research halted.***\n";
			return ret;
		}
		else
		{
			fstrm << "CLOSED, got magma.";
			std::cout << "CLOSED, got magma.";
			fstrm << std::endl;
			std::cout << std::endl;
		}

		// check associativity
		fstrm << std::endl; std::cout << std::endl;
		std::cout << "check associativity..." << std::endl;
		fstrm << "check associativity...";
		beg = clock();
		std::vector<int*> testAs = testAssociativity_pri1(table);
		fin = clock();
		int* ast = new int[testAs.size() * 3];
		for (int i = 0; i < testAs.size(); i++)
		{
			ast[i * 2 + 0] = testAs[i][0];
			ast[i * 2 + 1] = testAs[i][1];
			ast[i * 2 + 2] = testAs[i][2];
		}
		ret["associativity"] = std::make_pair(testAs.size(), ast);
		fstrm << "done" << std::endl;
		fstrm << "timer: " << fin - beg << std::endl;
		std::cout << "done" << std::endl;
		std::cout << "timer: " << fin - beg << std::endl;
		if (testAs.size())
		{
			fstrm << "NOT ASSOCIATIVITY! Operation on these triples not associative: ";
			std::cout << "NOT ASSOCIATIVITY! Operation on these triples not associative: ";
			for (auto e : testAs)
			{
				fstrm << '(' << e[0] << ',' << e[1] << ',' << e[2] << "); ";
				std::cout << '(' << e[0] << ',' << e[1] << ',' << e[2] << "); ";
			}
			fstrm << std::endl;
			std::cout << std::endl;
			//
			fstrm << "\n***Research halted.***\n";
			fstrm.close();
			std::cout << "\n***Research halted.***\n";
			return ret;
		}
		else
		{
			fstrm << "ASSOCIATIVE, got semigroup.";
			std::cout << "ASSOCIATIVE, got semigroup.";
			fstrm << std::endl;
			std::cout << std::endl;
		}

		for (std::pair<std::string, table_trans> item : items)
		{
			if (item.first.find("dempotent") != std::string::npos)
			{
				// find idempotents
				fstrm << std::endl; std::cout << std::endl;
				std::cout << "find idempotents..." << std::endl;
				fstrm << "find idempotents...";
				beg = clock();
				std::vector<int> idem = getIdempotent_pri1(table);
				fin = clock();
				ret["idempotent"] = std::make_pair(idem.size(), idem.data());
				fstrm << "done" << std::endl;
				fstrm << "timer: " << fin - beg << std::endl;
				std::cout << "done" << std::endl;
				std::cout << "timer: " << fin - beg << std::endl;
				if (idem.size())
				{
					fstrm << "GOT IDEMPOTENTS! In semigroup " << idem.size() << " idempotents: ";
					std::cout << "GOT IDEMPOTENTS! In semigroup " << idem.size() << " idempotents: ";
					for (int e : idem)
					{
						fstrm << e << "; ";
						std::cout << e << "; ";
					}
					fstrm << std::endl;
					std::cout << std::endl;
				}
				else
				{
					fstrm << "NOT IDEMPOTENT.";
					std::cout << "NOT IDEMPOTENT.";
					fstrm << std::endl;
					std::cout << std::endl;
				}
			}
			else if (item.first.find("dentity") != std::string::npos)
			{
				if ((item.first.find("wo") != std::string::npos)
					|| (item.first.find("oth") != std::string::npos))
				{
					// find two-side identity
					fstrm << std::endl; std::cout << std::endl;
					std::cout << "find two-side identity..." << std::endl;
					fstrm << "find two-side identity...";
					beg = clock();
					std::vector<int> tsi = getTSIdentity_pri1(table);
					fin = clock();
					ret["TSIdentity"] = std::make_pair(tsi.size(), tsi.data());
					fstrm << "done" << std::endl;
					fstrm << "timer: " << fin - beg << std::endl;
					std::cout << "done" << std::endl;
					std::cout << "timer: " << fin - beg << std::endl;
					if (tsi.size())
					{
						fstrm << "GOT TWO-SIDE IDENTITY! In semigroup: ";
						std::cout << "GOT TWO-SIDE IDENTITY! In semigroup: ";
						for (int e : tsi)
						{
							fstrm << e << "; ";
							std::cout << e << "; ";
						}
						fstrm << std::endl;
						std::cout << std::endl;
					}
					else
					{
						fstrm << "NOT TWO-SIDE IDENTITY.";
						std::cout << "NOT TWO-SIDE IDENTITY.";
						fstrm << std::endl;
						std::cout << std::endl;
					}
				}
				else if ((item.first.find("eft") != std::string::npos)
					|| (item.first.find("L") != std::string::npos)
					|| (item.first.find("l") != std::string::npos))
				{
					// find left identity
					fstrm << std::endl; std::cout << std::endl;
					std::cout << "find left identity..." << std::endl;
					fstrm << "find left identity...";
					beg = clock();
					std::vector<int> li = getLeftIdentity_pri1(table);
					fin = clock();
					ret["LeftIdentity"] = std::make_pair(li.size(), li.data());
					fstrm << "done" << std::endl;
					fstrm << "timer: " << fin - beg << std::endl;
					std::cout << "done" << std::endl;
					std::cout << "timer: " << fin - beg << std::endl;
					if (li.size())
					{
						fstrm << "GOT LEFT IDENTITY! In semigroup: ";
						std::cout << "GOT LEFT IDENTITY! In semigroup: ";
						for (int e : li)
						{
							fstrm << e << "; ";
							std::cout << e << "; ";
						}
						fstrm << std::endl;
						std::cout << std::endl;
					}
					else
					{
						fstrm << "NOT LEFT IDENTITY.";
						std::cout << "NOT LEFT IDENTITY.";
						fstrm << std::endl;
						std::cout << std::endl;
					}
				}
				else if ((item.first.find("ight") != std::string::npos)
					|| (item.first.find("R") != std::string::npos)
					|| (item.first.find("r") != std::string::npos))
				{
					// find right identity
					fstrm << std::endl; std::cout << std::endl;
					std::cout << "find right identity..." << std::endl;
					fstrm << "find right identity...";
					beg = clock();
					std::vector<int> ri = getRightIdentity_pri1(table);
					fin = clock();
					ret["RightIdentity"] = std::make_pair(ri.size(), ri.data());
					fstrm << "done" << std::endl;
					fstrm << "timer: " << fin - beg << std::endl;
					std::cout << "done" << std::endl;
					std::cout << "timer: " << fin - beg << std::endl;
					if (ri.size())
					{
						fstrm << "GOT RIGHT IDENTITY! In semigroup: ";
						std::cout << "GOT RIGHT IDENTITY! In semigroup: ";
						for (int e : ri)
						{
							fstrm << e << "; ";
							std::cout << e << "; ";
						}
						fstrm << std::endl;
						std::cout << std::endl;
					}
					else
					{
						fstrm << "NOT RIGHT IDENTITY.";
						std::cout << "NOT RIGHT IDENTITY.";
						fstrm << std::endl;
						std::cout << std::endl;
					}
				}
				else
				{
					fstrm << "WHAT KIND OF IDENTITY?";
					std::cout << "WHAT KIND OF IDENTITY?";
					fstrm << std::endl;
					std::cout << std::endl;
				}
			}
			else if ((item.first.find("eries") != std::string::npos)
				&& (item.first.find("ower") != std::string::npos))
			{
				// calculate power series
				fstrm << std::endl; std::cout << std::endl;
				std::cout << "calculate power series..." << std::endl;
				fstrm << "calculate power series...";
				beg = clock();
				std::map<int, std::pair<std::pair<int, int>,
					std::vector<int>>> ps = getPowerSeries_pri1(table);
				fin = clock();
				fstrm << "done" << std::endl;
				fstrm << "timer: " << fin - beg << std::endl;
				std::cout << "done" << std::endl;
				std::cout << "timer: " << fin - beg << std::endl;
				int tmpsize = 0;
				for (std::pair<int, std::pair<std::pair<int, int>,
					std::vector<int>>> e : ps)
				{
					if (e.second.first.first + e.second.first.second > tmpsize)
						tmpsize = e.second.first.first + e.second.first.second;
				}
				tmpsize += 2;
				int* pst = new int[table.first * tmpsize]();
				fstrm << "(index, period) power series in semigroup: " << std::endl;
				std::cout << "(index, period) power series in semigroup: " << std::endl;
				for (int i = 0; i < table.first; i++)
				{
					fstrm << "element " << i << " ";
					std::cout << "element " << i << " ";
					fstrm << "(" << ps[i].first.first << "," << ps[i].first.second << ") : ";
					std::cout << "(" << ps[i].first.first << "," << ps[i].first.second << ") : ";
					int j = 0;
					pst[i * tmpsize + j] = ps[i].first.first;
					j += 1;
					pst[i * tmpsize + j] = ps[i].first.second;
					j += 1;
					for (int e : ps[i].second)
					{
						fstrm << e << "; ";
						std::cout << e << "; ";
						pst[i * tmpsize + j] = e;
						j += 1;
					}
					fstrm << std::endl;
					std::cout << std::endl;
				}
				ret["PowerSeries"] = std::make_pair(table.first, pst);
			}
			else if (item.first.find("ommutat") != std::string::npos)
			{
				// get commutativity
				fstrm << std::endl; std::cout << std::endl;
				std::cout << "get commutativity..." << std::endl;
				fstrm << "get commutativity...";
				beg = clock();
				std::pair<int, std::vector<int*>> comm = haveCommutativity_pri1(table);
				fin = clock();
				fstrm << "done" << std::endl;
				fstrm << "timer: " << fin - beg << std::endl;
				std::cout << "done" << std::endl;
				std::cout << "timer: " << fin - beg << std::endl;
				int* ct = new int[comm.second.size() * 2 + 1]();
				ct[0] = comm.first;
				if (comm.first == comm.second.size())
				{
					fstrm << "GOT COMMUTATIVE SEMIGROUP!";
					std::cout << "GOT COMMUTATIVE SEMIGROUP!";
					fstrm << std::endl;
					std::cout << std::endl;
				}
				fstrm << comm.second.size() << " pairs are commutative: ";
				std::cout << comm.second.size() << " pairs are commutative: ";
				int i = 0;
				for (int* e : comm.second)
				{
					fstrm << "(" << e[0] << "," << e[1] << "); ";
					std::cout << "(" << e[0] << "," << e[1] << "); ";
					ct[1 + i * 2 + 0] = e[0];
					ct[1 + i * 2 + 1] = e[1];
				}
				fstrm << std::endl;
				std::cout << std::endl;
				ret["commutativity"] = std::make_pair(comm.second.size(), ct);
			}
			else if ((item.first.find("in") != std::string::npos)
				&& (item.first.find("ub") != std::string::npos)
				&& (item.first.find("emi") != std::string::npos)
				&& (item.first.find("roup") != std::string::npos))
			{
				// calculate generated min sub semi group
				fstrm << std::endl; std::cout << std::endl;
				std::cout << "calculate generated minsubsemigroup..." << std::endl;
				fstrm << "calculate generated minsubsemigroup...";
				beg = clock();
				table_trans gmssg = minSubSemiGroup_generated(table, item.second);
				fin = clock();
				fstrm << "done" << std::endl;
				fstrm << "timer: " << fin - beg << std::endl;
				std::cout << "done" << std::endl;
				std::cout << "timer: " << fin - beg << std::endl;
				ret["MinSubSemiGroup"] = gmssg;
				fstrm << "By {";
				std::cout << "By {";
				for (int i = 0; i < item.second.first; i++)
				{
					if (item.second.second[i])
					{
						fstrm << i << ",";
						std::cout << i << ",";
					}
				}
				fstrm << "} generated: {";
				std::cout << "} generated: {";
				for (int i = 0; i < gmssg.first; i++)
				{
					if (gmssg.second[i])
					{
						fstrm << i << ",";
						std::cout << i << ",";
					}
				}
				fstrm << "}" << std::endl;
				std::cout << "}" << std::endl;
			}
			else if ((item.first.find("inary") != std::string::npos)
				&& (item.first.find("elation") != std::string::npos)
				&& (item.first.find("hain") == std::string::npos))
			{
				// calculate binary relation table
				fstrm << std::endl; std::cout << std::endl;
				std::cout << "calculate binary relation table..." << std::endl;
				fstrm << "calculate binary relation table...";
				beg = clock();
				std::string brf = binaryRelation_table(table, testname + "_binaryRelationTable.csv", 'p');
				fin = clock();
				fstrm << "done" << std::endl;
				fstrm << "timer: " << fin - beg << std::endl;
				std::cout << "done" << std::endl;
				std::cout << "timer: " << fin - beg << std::endl;
				ret["BinaryRelation"] = std::make_pair(1, nullptr);
				fstrm << "binary relation table file name: " << brf << std::endl;
				std::cout << "binary relation table file name: " << brf << std::endl;
			}
			else if ((item.first.find("inary") != std::string::npos)
				&& (item.first.find("elation") != std::string::npos)
				&& (item.first.find("hain") != std::string::npos))
			{
				// calculate binary relation chain
				fstrm << std::endl; std::cout << std::endl;
				std::cout << "calculate binary relation chain..." << std::endl;
				fstrm << "calculate binary relation chain...";
				//std::pair<std::vector<int*>, bool*> brt = readBRTable_fromFile(table.second, brf); // no need
				beg = clock();
				std::vector<int*> brc = binaryRelation_2s2iChain(table,
					item.second.second[0], item.second.second[1], item.second.first);
				fin = clock();
				fstrm << "done" << std::endl;
				fstrm << "timer: " << fin - beg << std::endl;
				std::cout << "done" << std::endl;
				std::cout << "timer: " << fin - beg << std::endl;
				int* brct = new int[brc.size() * 2];
				fstrm << "Chain for {("
					<< item.second.second[0]
					<< ","
					<< item.second.second[1]
					<< "),"
					<< item.second.first
					<< "} is : ";
				std::cout << "Chain for {("
					<< item.second.second[0] << "," << item.second.second[1] << "),"
					<< item.second.first
					<< "} is : ";
				for (int i = 0; i < brc.size(); i++)
				{
					if (brc[i] != nullptr)
					{
						brct[i * 2 + 0] = brc[i][0];
						brct[i * 2 + 1] = brc[i][1];
						fstrm << "(" << brc[i][0] << "," << brc[i][1] << ");";
						std::cout << "(" << brc[i][0] << "," << brc[i][1] << ");";
					}
					else
					{
						brct[i * 2 + 0] = -2;
						brct[i * 2 + 1] = -2;
						fstrm << "(BAD STATE F);";
						std::cout << "(BAD STATE F);";
					}
				}
				ret["BinaryRelationChain"] = std::make_pair(brc.size(), brct);
				fstrm << std::endl;
				std::cout << std::endl;
			}
			else
			{
				fstrm << std::endl; std::cout << std::endl;
				fstrm << "?WHAT IS ITEM: " << item.first << std::endl;
				std::cout << "?WHAT IS ITEM: " << item.first << std::endl;
			}
		}
		// finish
		fstrm << "\n***Research finished.***\n";
		fstrm.close();
		std::cout << "\n***Research finished.***\n";
		return ret;
	}
	void research_example_ns()
	{// it's like version for deterministic version PRI
		Language* b = readLangFromString("{aaa}");
		std::map<std::string, table_trans> items;
		items["Commutativity"] = std::make_pair(1, nullptr);
		items["Idempotents"] = std::make_pair(1, nullptr);
		items["TwoSideIdentity"] = std::make_pair(1, nullptr);
		items["LeftIdentity"] = std::make_pair(1, nullptr);
		items["RightIdentity"] = std::make_pair(1, nullptr);
		items["PowerSerieses"] = std::make_pair(1, nullptr);
		items["MinSubSemiGroup{0,1}"] = std::make_pair(2, new int[2] {1, 1});
		items["BinaryRelationship"] = std::make_pair(1, nullptr);
		items["BinaryRelationshipChains"] = std::make_pair(2, new int[2] {0, 3});
		items["Dinner"] = std::make_pair(1, nullptr); // undefined item just return nothing
		// to be continued ...
		std::map<std::string, table_trans> res_res = intergrated_research_process_NS(
			b, "reasearch_example_ns_aaa", items, "BfollowA", 'l', "_");
		return;
	}

	//void sgpri_m3_Documentation(std::string s)
	//{
	//	std::string doc = "Not very clear input, documentation can't give an answer";
	//	if (s == "")
	//	{
	//		doc =
	//			"This is a simple introduction documentation for library <<sgpri_modified2.cpp>>.\n"
	//			"We will give a list of recommanded functions below.\n"
	//			"If you're interested in one of them, you can input its name into this documentation.\n"
	//			"Though, any function out of name space sgpri_m2:: is good.\n"
	//			"The functions in name space sgpri_m2:: is not suggested to use directly.\n"
	//			"******\n"
	//			"coverR_option\n"
	//			"choosePrefSuffLang\n"
	//			"getAllSublang\n"
	//			"findLanguageInSubLangs\n"
	//			"getTablePRI_twoLang\n"
	//			"getTablePRI_twoLang_printProgram\n"
	//			"operationSemigroup_F\n"
	//			"readLangFromString\n"
	//			"readTable_opsg_fromFile\n"
	//			"uniform_table\n"
	//			"****** and here are recommanded data types, which are shorter and convenient to use\n"
	//			"typePRI_2\n"
	//			"typePRI_1\n"
	//			;
	//	}
	//	else if (s == "coverR_option")
	//	{
	//		doc = "function <coverR_option>:\n"
	//			"is for choosing type of Phi-function of cover relation\n"
	//			"accepts a human-read string <std::string option>\n"
	//			"gives char of its inner represent <char>\n"
	//			"usually it's used in functions, user needn't use it\n";
	//	}
	//	else if (s == "choosePrefSuffLang")
	//	{
	//		doc = "function <choosePrefSuffLang>:\n"
	//			"is for generating oprefix or suffix language by type of Phi-function of cover relation\n"
	//			"accepts language itself <Language * l>\n"
	//			"and human-read type of Phi <std::string option>\n"
	//			"gives result language <Language*>\n"
	//			"usually it's used in functions, user needn't use it\n";
	//	}
	//	//std::pair<
	//	//	std::vector<std::pair<int, std::vector<int*>>>,
	//	//	std::vector<Language*>> getAllSublang(Language * lan);
	//	//int findLanguageInSubLangs(Language * l, const std::pair<
	//	//	std::vector<std::pair<int, std::vector<int*>>>, std::vector<Language*>>&subl);
	//	//std::pair<
	//	//	std::pair<std::vector<Language*>, std::vector<Language*>>,
	//	//	int*> getTablePRI_twoLang(
	//	//		Language * la, Language * lb, std::string option = "BfollowA", bool onlyAttainable = true);
	//	//void getTablePRI_twoLang_printProgram(
	//	//	std::pair<std::pair<std::vector<Language*>, std::vector<Language*>>, int*> test,
	//	//	char len = 'l', std::string ep = "_");
	//	//Language* operationSemigroup_F(
	//	//	Language * base, Language * x, Language * y,
	//	//	std::string option = "BfollowA");
	//	//Language* readLangFromString(std::string l,
	//	//	std::string ep,
	//	//	Alphabet * defaul = nullptr);
	//	//std::pair<
	//	//	std::pair<Language*, std::vector<Language*>>,
	//	//	std::pair<int, int*>> readTable_opsg_fromFile( //?
	//	//		std::string filename, Language * bl, std::string option = "BfollowA");
	//	//std::string tablePRI_toFile(
	//	//	std::pair<std::pair<std::vector<Language*>, std::vector<Language*>>, int*> table,
	//	//	std::string filename, char len = 'l', std::string ep = "_");
	//	//std::pair<
	//	//	std::pair<std::vector<Language*>, std::vector<Language*>>,
	//	//	int*> readTable_opsg_fromFile(
	//	//		std::string filename, std::set<std::string> nw);
	//	//std::pair<
	//	//	std::pair<std::vector<Language*>, std::vector<Language*>>,
	//	//	int*> uniform_table(
	//	//		std::pair<
	//	//		std::pair<Language*, std::vector<Language*>>,
	//	//		std::pair<int, int*>>&table);
	//	//using typePRI_2 = std::pair<std::pair<std::vector<Language*>, std::vector<Language*>>, int*>;
	//	//using typePRI_1 = std::pair<std::pair<Language*, std::vector<Language*>>, std::pair<int, int*>>;
	//	//typePRI_1 tablePRI_allSubsetOPrefBaseLanguage_opsg(Language * bl, std::string option = "BfollowA");
	//	//void tablePRI_aSsOPBL_opsg_print(typePRI_1 t, char len = 'l', std::string ep = "_");
	//	//std::string tablePRI_aSsOPBL_opsg_toFile(
	//	//	Language * bl, std::string filename,
	//	//	std::string option = "BfollowA", char len = 'l', std::string ep = "_");
	//	//typePRI_1 tablePRI_groupoid(Language * bl, std::string option = "BfollowA");
	//	//void tablePRI_groupoid_print(typePRI_1 t, char len = 'l', std::string ep = "_");
	//	//std::string tablePRI_groupoid_toFile(Language * bl, std::string filename,
	//	//	std::string option = "BfollowA", char len = 'l', std::string ep = "_");
	//	//using pri1_table = std::pair<int, int*>;
	//	//using table_trans = std::pair<int, int*>;
	//	//template<typename T> void tablePRI_print(T t, char len = 'l', std::string ep = "_");
	//
	//	//Language* lang_union(Language * a, Language * b);
	//	//Language* lang_intersect(Language * a, Language * b);
	//	//Language* lang_multiple(Language * a, Language * b);
	//	//table_trans compose_trans(const table_trans & t1, const table_trans & t2);
	//	//table_trans get_trans_fromPRI2(const typePRI_2 & t, Language * l);
	//	//table_trans get_trans_fromPRI2(const typePRI_2 & t, std::string w);
	//	//Language* get_word_byTrans_fromPRI2(const typePRI_2 & t, const table_trans & m);
	// 
	//	//std::vector<int*> testClosure_pri1(const pri1_table & table);
	//	//std::vector<int*> testClosure_pri1(const typePRI_1 & table);
	//	//std::pair<std::vector<Language*>, std::vector<int*>> testClosure_pri1(
	//	//	Language * bl, std::string option = "BfollowA", bool loading = true);
	//	//std::vector<int*> testAssociativity_pri1(const pri1_table & table);
	//	//std::vector<int*> testAssociativity_pri1(const typePRI_1 & table);
	//	//std::pair<std::vector<Language*>, std::vector<int*>> testAssociativity_pri1(
	//	//	Language * bl, std::string option = "BfollowA", bool loading = true);
	//	//std::vector<int> getTSIdentity_pri1(const pri1_table & table);
	//	//std::vector<int> getLeftIdentity_pri1(const pri1_table & table);
	//	//std::vector<int> getRightIdentity_pri1(const pri1_table & table);
	//	//std::vector<int> getIdempotent_pri1(const pri1_table & table);
	//	//std::map<int, std::pair<std::pair<int, int>,
	//	//	std::vector<int>>> getPowerSeries_pri1(const pri1_table & table);
	//	//std::pair<int, std::vector<int*>> haveCommutativity_pri1(const pri1_table & table);
	//	//bool  binaryRelation_22s1i(pri1_table p, int q1, int q2, int q3, int q4, int a);
	//	//bool  binaryRelation_22s(pri1_table p, int q1, int q2, int q3, int q4);
	//	//std::pair<std::vector<int*>, bool*>  binaryRelation_table(pri1_table p, char prnt = '\0');
	//	//std::string  binaryRelation_table(pri1_table p, std::string filename, char prnt = 'p');
	//	//std::pair<std::vector<int*>, bool*>  readBRTable_fromFile(pri1_table p, std::string filename, char prnt = '\0');
	//	//std::vector<int*>  binaryRelation_2s2iChain(pri1_table p, int p1, int p2, int input = 1);
	//	//std::vector<int> minSubSemiGroup_generated(pri1_table t, std::vector<int> gen);
	//	//table_trans minSubSemiGroup_generated(pri1_table t, table_trans gen);
	//	//table_trans maxSubGroupSemigroup(pri1_table t);
	// 
	//	//std::map<std::string, table_trans> intergrated_research_process(
	//	//	Language * bl, std::string testname, std::map<std::string, table_trans> items,
	//	//	std::string option = "BfollowA", char len = 'l', std::string ep = "_");
	//	//void research_example();
	// 
	//	//using typeNSPRI_2 = std::pair<std::pair<std::vector<Language*>, std::vector<Language*>>, int**>;
	//	//using typeNSPRI_1 = std::pair<std::vector<Language*>, int**>;
	//	//std::pair<
	//	//	std::pair<std::vector<Language*>, std::vector<Language*>>,
	//	//	int**> getTableNSPRI_twoLang(
	//	//		Language * la, Language * lb, std::string option = "BfollowA", bool onlyAttainable = true);
	//	//void getTableNSPRI_printProgram(
	//	//	std::pair<std::pair<std::vector<Language*>, std::vector<Language*>>, int**> test,
	//	//	char len = 'l', std::string ep = "_");
	//	//std::pair<std::vector<Language*>, int**> getTableNSPRI_oneLang(
	//	//	Language * lb, std::string option = "BfollowA");
	//	//void getTableNSPRI_printProgram(
	//	//	std::pair<std::vector<Language*>, int**> test,
	//	//	char len = 'l', std::string ep = "_");
	//	//std::string tableNSPRI_toFile(
	//	//	std::pair<std::pair<std::vector<Language*>, std::vector<Language*>>, int**> test,
	//	//	std::string filename, char len = 'l', std::string ep = "_");
	//	//std::string tableNSPRI_toFile(std::pair<std::vector<Language*>, int**> test,
	//	//	std::string filename, char len = 'l', std::string ep = "_");
	//	//typeNSPRI_2 readTableNSPRI_fromFile(std::string filename, std::set<std::string> nw);
	//	//typePRI_2 getPRI_fromNSPRI(typeNSPRI_2 t);
	//	//pri1_table getPRI_fromNSPRI(typeNSPRI_1 t);
	//	//typePRI_1 getPRI_fromNSPRI(typeNSPRI_1 t, Language * b);
	//	//std::map<std::string, table_trans> intergrated_research_process_NS(
	//	//	Language * bl, std::string testname, std::map<std::string, table_trans> items,
	//	//	std::string option = "BfollowA", char len = 'l', std::string ep = "_");
	//	std::cout << doc << std::endl;
	//}

	// new part, not in documentation

	// seem like relating to semilattice & parallel computation
	// find triples <input, language1, language2> that:
	// if PHI(input, language1) = language1' , PHI(input, language2) = language2'
	// then PHI(input, union of language1 and language2) = union of language1' and language2'
	// i.e. they "conservate joining under Phi_AminusB"
	std::pair<std::pair<std::vector<Language*>, std::vector<Language*>>,
		std::vector<int* >> opsgConservatedJoinTriple(typePRI_2 t,
			const std::pair<std::vector<std::pair<int, std::vector<int*>>>,
			std::vector<Language*>>&subl,
			bool re)
	{// find triples <input,language,language> conservating joining under PHI
		// here provided model_subset "subl", faster
		std::vector<int*> cmbnModel;
		if (re) cmbnModel = myCombinationNum(t.first.first.size(), 2, true);
		else cmbnModel = myCmbntnN_noRe(t.first.first.size(), 2);
		std::vector<int*> res;
		for (int* e : cmbnModel)
		{
			for (int i = 0; i < t.first.second.size(); i++)
			{
				if ((findLanguageInSubLangs(lang_union(t.first.first[e[0]], t.first.first[e[1]]),
					subl) >= 0) &&
					lang_union(t.first.first[t.second[e[0] * t.first.second.size() + i]],
						t.first.first[t.second[e[1] * t.first.second.size() + i]])->compareLanguage(
							t.first.first[t.second[findLanguageInSubLangs(lang_union(t.first.first[e[0]], t.first.first[e[1]]),
								subl) * t.first.second.size() + i]]
						)) res.push_back(new int[3] {i, e[0], e[1]});
			}
		}
		return std::make_pair(t.first, res);
	}
	std::pair<std::pair<std::vector<Language*>, std::vector<Language*>>,
		std::vector<int*>> opsgConservatedJoinTriple(typePRI_2 t, bool re)
	{// find triples <input,language,language> conservating joining under PHI
		// without model_subset
		std::vector<int*> cmbnModel;
		if (re) cmbnModel = myCombinationNum(t.first.first.size(), 2, true);
		else cmbnModel = myCmbntnN_noRe(t.first.first.size(), 2);
		std::vector<int*> res;
		for (int* e : cmbnModel)
		{
			for (int i = 0; i < t.first.second.size(); i++)
			{
				if ((findLanguageInSubLangs(lang_union(t.first.first[e[0]], t.first.first[e[1]]),
					t.first.first) >= 0) &&
					lang_union(t.first.first[t.second[e[0] * t.first.second.size() + i]],
						t.first.first[t.second[e[1] * t.first.second.size() + i]])->compareLanguage(
							t.first.first[t.second[findLanguageInSubLangs(lang_union(t.first.first[e[0]], t.first.first[e[1]]),
								t.first.first) * t.first.second.size() + i]]
						)) res.push_back(new int[3] {i, e[0], e[1]});
			}
		}
		return std::make_pair(t.first, res);
	}
	std::pair<std::pair<std::vector<Language*>, std::vector<Language*>>,
		std::vector<int*>> opsgConservatedJoinTriple(typePRI_2 t, Language* b, bool re)
	{// find triples <input,language,language> conservating joining under PHI
		// provide base language so can use "Phi_AminusB"
		std::vector<int*> cmbnModel;
		if (re) cmbnModel = myCombinationNum(t.first.first.size(), 2, true);
		else cmbnModel = myCmbntnN_noRe(t.first.first.size(), 2);
		std::vector<int*> res;
		for (int* e : cmbnModel)
		{
			for (int i = 0; i < t.first.second.size(); i++)
			{
				if (lang_union(t.first.first[t.second[e[0] * t.first.second.size() + i]],
					t.first.first[t.second[e[1] * t.first.second.size() + i]])->compareLanguage(
						Phi_AminusB(t.first.second[i], b, lang_union(t.first.first[e[0]], t.first.first[e[1]]))
					)) res.push_back(new int[3] {i, e[0], e[1]});
			}
		}
		return std::make_pair(t.first, res);
	}

}
