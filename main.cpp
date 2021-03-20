#include<bits/stdc++.h>
using namespace std;


vector<string> rev_register_map = {"$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3","$t0","$t1","$t2","$t3","$t4"
                                    ,"$t5","$t6","$t7","$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7","$t8","$t9"
                                    ,"$k0","$k1","$gp","$sp","$fp","$ra"};
unordered_map<string,int> register_map;
unordered_map<string,int> label_map;
unordered_set<string> labels_used;
int INSTR_NUM = 0;
int INSTR_MEM = 0;

void initializeRegisterMap(){
    register_map["$zero"]=0;
    register_map["$at"]=1;
    register_map["$v0"]=2;
    register_map["$v1"]=3;
    register_map["$a0"]=4;
    register_map["$a1"]=5;
    register_map["$a2"]=6;
    register_map["$a3"]=7;
    register_map["$t0"]=8;
    register_map["$t1"]=9;
    register_map["$t2"]=10;
    register_map["$t3"]=11;
    register_map["$t4"]=12;
    register_map["$t5"]=13;
    register_map["$t6"]=14;
    register_map["$t7"]=15;
    register_map["$s0"]=16;
    register_map["$s1"]=17;
    register_map["$s2"]=18;
    register_map["$s3"]=19;
    register_map["$s4"]=20;
    register_map["$s5"]=21;
    register_map["$s6"]=22;
    register_map["$s7"]=23;
    register_map["$t8"]=24;
    register_map["$t9"]=25;
    register_map["$k0"]=26;
    register_map["$k1"]=27;
    register_map["$gp"]=28;
    register_map["$sp"]=29;
    register_map["$fp"]=30;
    register_map["$ra"]=31;
}

vector<int> register_File(32,0);
//vector<int> memory(1048576,0);
char memory[1048576] = {'a'};


struct Instruction {
    string command;
    vector<string> param;
    int lineNumber;
    int execution_count;
    string str;
    int mem_address;
};

vector<Instruction> program;
vector<string> temp_paramList;
string temp_command;


bool validReg(string s){
    if(s=="$at" || s=="$sp" || register_map.find(s)==register_map.end()){
        return false;
    }
    return true;
}

bool validLabel(string s){
    if(s==""){
        return false;
    }
    if(! (s[0]=='_' || (s[0]>='a'&&s[0]<='z') || (s[0]>='A'&&s[0]<='Z') )){
        return false;
    }
    for(char ch:s){
        if(! (ch=='_' || (ch>='a'&&ch<='z') || (ch>='A'&&ch<='Z') || (ch>='0'&&ch<='9') )){
            return false;
        }
    }

    return true;
}

bool isNum(string s){
    if(s==""){
        return false;
    }

    int n=s.size();
    if(n==1 && s=="-"){
        return false;
    }
    for(int i=0;i<n;i++){
        if(i==0){
            if(s[i]=='-'){
                continue;
            }
        }
        if(! (s[i]<='9'&&s[i]>='0') ){
            return false;
        }
    }


    return true;
}

bool AddSubMulSlt_check(vector<string>& tokens){
    int n=tokens.size();
    if(n!=4){
        return false;
    }

    if(!validReg(tokens[1])){
        return false;
    }
    if(!validReg(tokens[2])){
        return false;
    }
    if(!validReg(tokens[3])){
        return false;
    }
    temp_paramList = {tokens[1],tokens[2],tokens[3]};
    temp_command = tokens[0];
    return true;
}

bool Branch_check(vector<string>& tokens){
    int n = tokens.size();
    if(n!=4){
        return false;
    }

    if(!validReg(tokens[1])){
        return false;
    }
    if(!validReg(tokens[2])){
        return false;
    }

    if(!validLabel(tokens[3])){
        return false;
    }
    labels_used.insert(tokens[3]);
    temp_paramList = {tokens[1],tokens[2],tokens[3]};
    temp_command = tokens[0];

    return true;
}

bool J_check(vector<string>& tokens){
    int n=tokens.size();
    if(n!=2){
        return false;
    }

    if(!validLabel(tokens[1])){
        return false;
    }
    labels_used.insert(tokens[1]);
    temp_paramList = {tokens[1]};
    temp_command = tokens[0];

    return true;
}

bool LwSw_check(vector<string> tokens){
    int n=tokens.size();
    if(n!=3){
        return false;
    }

    if(!validReg(tokens[1])){
        return false;
    }

    // address should be numeric number.
    if(!isNum(tokens[2])){
        int sz=tokens[2].size();
        if(sz<6){
            return false;
        }
        string reg_part = tokens[2].substr(sz-5,5);
        if(reg_part != "($sp)"){
            return false;
        }
        string num_part = tokens[2].substr(0,sz-5);
        if(!isNum(num_part)){
            return false;
        }
        temp_paramList = {tokens[1],num_part,reg_part};
        temp_command = tokens[0];
        return true;
    }

    temp_paramList = {tokens[1],tokens[2]};
    temp_command = tokens[0];

    return true;
}

bool Addi_check(vector<string>& tokens){
    int n=tokens.size();
    if(n!=4){
        return false;
    }

    if(!validReg(tokens[1])){
        return false;
    }
    if(!validReg(tokens[2])){
        return false;
    }

    if(!isNum(tokens[3])){
        return false;
    }

    temp_paramList = {tokens[1],tokens[2],tokens[3]};
    temp_command = tokens[0];

    return true;
}

bool Label_check(vector<string>& tokens){
    int n=tokens.size();
    if(n>2){
        return false;
    }
    string lab;
    if(n==2){
        if(! (tokens[1]==":") ){
            return false;
        }
        else{
            if(!validLabel(tokens[0])){
                return false;
            }
        }
        lab = tokens[0];
    }
    else if(n==1){
        int sz=tokens[0].size();
        if(tokens[0][sz-1]!=':'){
            return false;
        }
        if(!validLabel(tokens[0].substr(0,sz-1))){
            return false;
        }
        lab = tokens[0].substr(0,sz-1);
    }

    // same label occurring twice in the program.
    if(label_map.find(lab) != label_map.end()){
        return false;
    }
    label_map[lab] = INSTR_NUM;
    temp_paramList = {};
    temp_command = lab;
    return true;
}


bool validate(string& s){
    int n=s.size();
    char str[n+1];
    strcpy(str,s.c_str());
    char* temp = strtok(str," \t,");
    vector<string> tokens;
    while(temp != NULL){
        tokens.push_back(temp);
        temp = strtok(NULL," \t,");
    }
    n = tokens.size();
    if(n==0){
        return false;
    }

    if(tokens[0] == "add"){
        return AddSubMulSlt_check(tokens);
    }
    else if(tokens[0] == "sub"){
        return AddSubMulSlt_check(tokens);
    }
    else if(tokens[0] == "mul"){
        return AddSubMulSlt_check(tokens);
    }
    else if(tokens[0] == "beq"){
        return Branch_check(tokens);
    }
    else if(tokens[0] == "bne"){
        return Branch_check(tokens);
    }
    else if(tokens[0] == "slt"){
        return AddSubMulSlt_check(tokens);
    }
    else if(tokens[0] == "j"){
        return J_check(tokens);
    }
    else if(tokens[0] == "lw"){
        return LwSw_check(tokens);
    }
    else if(tokens[0] == "sw"){
        return LwSw_check(tokens);
    }
    else if(tokens[0] == "addi"){
        return Addi_check(tokens);
    }
    else{
        // case of label
        return Label_check(tokens);
    }

    return false;
}

void tokenise(int lineNum,string str){
    INSTR_NUM++;
    Instruction temp;
    temp.command = temp_command;
    temp.param = temp_paramList;
    temp.lineNumber = lineNum;
    temp.execution_count = 0;
    temp.str = str;
    temp.mem_address = INSTR_MEM;
    program.push_back(temp);

    if(temp_paramList.size()!=0){
        INSTR_MEM+=4;
    }
}

int reg(string s){
    return register_map[s];
}

int register_file(int p){
    return register_File[p];
}

int goto_lab(string s){
    return label_map[s];
}

bool outOfBounds(int address){
    if(address > 1048575){
        return true;
    }
    if(address < INSTR_MEM){
        return true;
    }
    if(address%4 !=0 ){
        return true;
    }
    return false;
}

int add_overflow(int v3,int v1,int v2){
    if(v1>0 && v2>0 && v3<0){
        return true;
    }
    else if(v1<0 && v2<0 && v3>0){
        return true;
    }
    return false;
}

int sub_overflow(int v3,int v1,int v2){
    if(v1>0 && v2<0 && v3<0){
        return true;
    }
    else if(v1<0 && v2>0 && v3>0){
        return true;
    }
    return false;
}

int mul_overflow(int v3,int v1,int v2){
    if(v1==0 || v2==0){
        return false;
    }
    if(v2 == v3/v1){
        return false;
    }
    return true;
}

bool isEmpty(string s){
    for(char ch:s){
        if(ch!=' '&&ch!='\t'){
            return false;
        }
    }
    return true;
}

int main(){

    initializeRegisterMap();
    register_File[29] = 1048576;

    ifstream fin;
    fin.open("input.txt");
    int lineNum=1;
    INSTR_NUM = 0;
    string str;
    while(getline(fin,str)){
        if(isEmpty(str)){
            lineNum++;
            continue;
        }

        bool correct_syntax = validate(str);
        if(!correct_syntax){
            cout<<"Syntax error: At line number: "<<lineNum<<" : "<<str<<endl;
            return -1;
        }

        tokenise(lineNum,str);

        lineNum++;
    }
    for(auto str:labels_used){
        if(label_map.find(str) == label_map.end()){
            cout<<"Syntax Error: Undefined label:"<<str<<"\n";
            return -1;
        }
    }


    fin.close();

    vector<int> instr_exec_cnt(10,0);
    int instruction_number=0;
    int TOTAL_INSTR_EXECUTED = 0;
    while(instruction_number < INSTR_NUM){
        string cmd = program[instruction_number].command;
        vector<string> arg = program[instruction_number].param;
        int l = program[instruction_number].mem_address;
        program[instruction_number].execution_count++;
        bool skip=false;
        if(cmd == "add"){
            instr_exec_cnt[0]++;
            int dst = reg(arg[0]);
            int p1 = reg(arg[1]);
            int p2 = reg(arg[2]);
            if(dst!=-1){
                int val1 = register_file(p1);
                int val2 = register_file(p2);
                int val3 = val1 + val2;
                if(add_overflow(val3,val1,val2)){
                    cout<<"Memory Address: "<<l<<" : Calculation overflow detected!!\n";
                    return -1;
                }
                register_File[dst] = val3;
            }
        }
        else if(cmd == "sub"){
            instr_exec_cnt[1]++;
            int dst = reg(arg[0]);
            int p1 = reg(arg[1]);
            int p2 = reg(arg[2]);
            if(dst!=0){
                int val1 = register_file(p1);
                int val2 = register_file(p2);
                int val3 = val1 - val2;
                if(sub_overflow(val3,val1,val2)){
                    cout<<"Memory Address: "<<l<<" : Calculation overflow detected!!\n";
                    return -1;
                }
                register_File[dst] = val3;
            }
        }
        else if(cmd == "mul"){
            instr_exec_cnt[2]++;
            int dst = reg(arg[0]);
            int p1 = reg(arg[1]);
            int p2 = reg(arg[2]);
            if(dst!=0){
                int val1 = register_file(p1);
                int val2 = register_file(p2);
                int val3 = val1 * val2;
                if(mul_overflow(val3,val1,val2)){
                    cout<<"Memory Address: "<<l<<" : Calculation overflow detected!!\n";
                    return -1;
                }
                register_File[dst] = val3;
            }
        }
        else if(cmd == "beq"){
            instr_exec_cnt[3]++;
            int p1 = reg(arg[0]);
            int p2 = reg(arg[1]);
            if(register_file(p1) == register_file(p2)){
                // change value of i to the instruction of label:arg[2]
                instruction_number = goto_lab(arg[2]);
            }
        }
        else if(cmd == "bne"){
            instr_exec_cnt[4]++;
            int p1 = reg(arg[0]);
            int p2 = reg(arg[1]);
            if(register_file(p1) != register_file(p2)){
                // store value of $ra register to this statement.
                // change value of i to the instruction of label:arg[2]
                instruction_number = goto_lab(arg[2]);
            }
        }
        else if(cmd == "slt"){
            instr_exec_cnt[5]++;
            int dst = reg(arg[0]);
            int p1 = reg(arg[1]);
            int p2 = reg(arg[2]);
            if(dst!=0 && register_file(p1) < register_file(p2)){
                register_File[dst] = 1;
            }
            else if(dst!=0){
                register_File[dst] = 0;
            }
        }
        else if(cmd == "j"){
            instr_exec_cnt[6]++;
            // change value of i to the label:arg[0]
             instruction_number = goto_lab(arg[0]);
        }
        else if(cmd == "lw"){
            instr_exec_cnt[7]++;
            int r = reg(arg[0]);
            int p = stoi(arg[1]);
            int a=0;
            if(arg.size()==3){
                a=register_file(reg(arg[2]));
            }
            // load value from memory[p] to register_file[r];
            if(r!=0){
                int val = INSTR_MEM + p +a;
                if(outOfBounds(val)){
                    cout<<"Memory Address:"<<l<<":Attempt to access memory failed!! Terminating Execution!\n";
                    return -1;
                }
                //register_File[r] = memory[val];
                memcpy(&register_File[r],memory+val,4);
            }
        }
        else if(cmd == "sw"){
            instr_exec_cnt[8]++;
            int r = reg(arg[0]);
            int p = stoi(arg[1]);
            int a=0;
            if(arg.size()==3){
                a=register_file(reg(arg[2]));
            }
            // store value in memory[p] from register_file[r];
            // memory form 0 to INSTR_MEM -1 is used for instructions, after that for data.
            int val = INSTR_MEM + p +a;
            if(outOfBounds(val)){
                cout<<"Memory Address:"<<l<<":Attempt to access memory out of bounds failed!! Terminating Execution!\n";
                return -1;
            }
            //memory[val] = register_file(r);
            int value = register_file(r);
            memcpy(memory+val,(char*)&value,4);
        }
        else if(cmd == "addi"){
            instr_exec_cnt[9]++;
            int dst = reg(arg[0]);
            int r = reg(arg[1]);
            int c = stoi(arg[2]);
            if(dst!=0){
                int val1 = register_file(r);
                int val2 = c;
                int val3 = val1 + val2;
                if(add_overflow(val3,val1,val2)){
                    cout<<"Memory Address: "<<l<<" : Calculation overflow detected!!\n";
                    return -1;
                }
                register_File[dst] = register_file(r) + c;
            }
        }
        else{
            // case of label
            skip = true;
        }
        instruction_number++;
        TOTAL_INSTR_EXECUTED++;
        if(skip){
            continue;
        }

        // print status of register file
        for(int j=0;j<32;j++){
            cout<<rev_register_map[j]<<": ";
            cout<<hex<<register_file(j)<<dec<<",";
        }
        cout<<"\n";

    }
    cout<<"\n";
    cout<<"Number of clock cycles: "<<TOTAL_INSTR_EXECUTED<<"\n";
    cout<<"Number of times each instruction got executed: \n";
    for(int i=0;i<INSTR_NUM;i++){
        cout<<program[i].str<<": "<<program[i].execution_count<<"\n";
    }

    vector<string> ins = {"add","sub","mul","beq","bne","slt","j","lw","sw","addi"};
    cout<<"Number of times each kind of instruction is executed \n";
    for(int i=0;i<10;i++){
        cout<<ins[i]<<" : "<<instr_exec_cnt[i]<<"\n";
    }

    return 0;
}
