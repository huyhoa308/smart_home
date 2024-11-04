#include<iostream>
#include<fstream>
#include<cstring>
#include<iomanip>
using namespace std;

class student{
    string  lastName,
			firstName,
			fullName,
            id,
            cclass,
            phoneNumber,
            email,
            idCardNumber,
            bankAccountNumber,
            homeTown,
            address;
    size_t numberOfSubjects;
    float *scores;
public:
    student(){
        lastName="";
        firstName="";
        id="";
        cclass="";
        phoneNumber="";
        email="";
        idCardNumber="";
        bankAccountNumber="";
        homeTown="";
        address="";
        numberOfSubjects=0;
    }
    student(string lastName,string firstName,string id,string cclass,string phoneNumber,string email,string idCardNumber,string bankAccountNumber,string homeTown,string address,size_t numberOfSubjects,float scores[]){
        this->lastName=lastName;
		this->firstName=firstName;
        this->id=id;
        this->cclass=cclass;
        this->phoneNumber=phoneNumber;
        this->email=email;
        this->idCardNumber=idCardNumber;
        this->bankAccountNumber=bankAccountNumber;
        this->homeTown=homeTown;
        this->address=address;
        this->numberOfSubjects=numberOfSubjects;
        this->scores=new float[numberOfSubjects];
        for(int a=0;a<numberOfSubjects;a++){
        	this->scores[a]=scores[a];
		}
		fullName=lastName+" "+firstName;
    }
    float averageScore(){
    	float sum=0;
    	for(int i=0;i<numberOfSubjects;i++){
    		sum=sum+scores[i];
		}
		return sum/numberOfSubjects;
	}
	string rate(){
		if(averageScore()>=9.5){
			return "Xuat sac";
		}
		else if(averageScore()>=8.5){
			return "Gioi";
		}
		else if(averageScore()>=7){
			return "Kha";
		}
		else if(averageScore()>=5.5){
			return "Trung binh";
		}
		else if(averageScore()>=4){
			return "Trung binh yeu";
		}
		else{
			return "Yeu";
		}
	}
	void setName(string lastName,string firstName){
		this->lastName=lastName;
		this->firstName=firstName;
		fullName=lastName+" "+firstName;
	}
	void setID(string id){
		this->id=id;
	}
	void setClass(string cclass){
		this->cclass=cclass;
	}
	void setPhoneNumber(string phoneNumber){
		this->phoneNumber=phoneNumber;
	}
	void setEmail(string email){
		this->email=email;
	}
	void setIDCardNumber(string idCardNumber){
		this->idCardNumber=idCardNumber;
	}
    void setBankAccountNumber(string bankAccountNumber){
	    this->bankAccountNumber=bankAccountNumber;
	}
	void setHomeTown(string homeTown){
        this->homeTown=homeTown;
	}
    void setAddress(string address){
	    this->address=address;
	}
	void setScores(float scores[],size_t addNumberOfSubjects){
		float *tmp=new float [numberOfSubjects];
		for(int a=0;a<numberOfSubjects;a++){
			tmp[a]=this->scores[a];
		}
        numberOfSubjects=numberOfSubjects+addNumberOfSubjects;
        this->scores=new float[numberOfSubjects];
        int i=0;
        for(int a=0;a<numberOfSubjects-addNumberOfSubjects;a++){
        	this->scores[i]=tmp[a];
        	i++;
		}
		for(int a=0;a<addNumberOfSubjects;a++){
			this->scores[i]=scores[a];
			i++;
		}
	}
	string getFirstName(){
		return firstName;
	}
	string getLastName(){
		return lastName;
	}
	string getFullName(){
		return fullName;
	}
	string getID(){
		return id;
	}
	string getClass(){
		return cclass;
	}
	string getPhoneNumber(){
		return phoneNumber;
	}
	string getEmail(){
		return email;
	}
	string getIDCardNumber(){
		return idCardNumber;
	}
	string getBankAccountNumber(){
		return bankAccountNumber;
	}
	string getHomeTown(){
		return homeTown;
	}
	string getAddress(){
		return address;
	}
	size_t getNumberOfSubjects(){
		return numberOfSubjects;
	}
	float* getScores(){
		return scores;
	}
	void display(){
		cout<<setfill('-')<<setw(50)<<""<<endl;
		cout<<"Ho va ten: "<<fullName<<endl;
		cout<<"ID: "<<id<<endl;
		cout<<"Lop: "<<cclass<<endl;
		cout<<"So dien thoai: "<<phoneNumber<<endl;
		cout<<"Email: "<<email<<endl;
		cout<<"So CMND: "<<idCardNumber<<endl;
		cout<<"So tai khoang ngang hang: "<<bankAccountNumber<<endl;
		cout<<"Que quan: "<<homeTown<<endl;
		cout<<"Dia chi: "<<address<<endl;
		cout<<"So mon: "<<setprecision(3)<<numberOfSubjects<<endl;
		cout<<"Diem: ";
		for (int a=0;a<numberOfSubjects;a++){
			cout<<setfill(' ')<<setw(5)<<left<<scores[a];
		}
		cout<<endl;
		cout<<"Diem trung binh: "<<averageScore()<<endl;
		cout<<"Xep loai: "<<rate()<<endl;
	}
};


void sortName(student students[],size_t numberOfStudents){
	for (int i=0;i<numberOfStudents-1;i++){
		for (int j=i+1;j<numberOfStudents;j++){
			if(students[j].getFirstName().compare(students[i].getFirstName())<0){
				swap(students[i],students[j]);
			}
			if(students[j].getFirstName().compare(students[i].getFirstName())==0){
				if(students[j].getLastName().compare(students[i].getLastName())<0){
					swap(students[i],students[j]);
				}
			}
		}
	}
}



void sortID(student students[],size_t numberOfStudents){
	for (int i=0;i<numberOfStudents-1;i++){
		for (int j=i+1;j<numberOfStudents;j++){
			if(students[j].getID().compare(students[i].getID())<0){
				swap(students[i],students[j]);
			}
		}
	}
}


void sortFullName(student students[],size_t numberOfStudents){
	for (int i=0;i<numberOfStudents-1;i++){
		for (int j=i+1;j<numberOfStudents;j++){
			if(students[j].getFullName().compare(students[i].getFullName())<0){
				swap(students[i],students[j]);
			}
		}
	}
}


void input(ifstream& ifInputStudent,student students[],size_t& numberOfStudents){
    string  lastName,
    		firstName,
            id,
            cclass,
            phoneNumber,
            email,
            idCardNumber,
            bankAccountNumber,
            homeTown,
            address;
    size_t numberOfSubjects=0;
    float *scores;
    if(!ifInputStudent.is_open()){
        cout<<"File doesn't exit!";
    }else{
        size_t i=0;
        while(!ifInputStudent.eof()){
        	numberOfStudents++;
        	fflush(stdin);
            getline(ifInputStudent,lastName);
            if(lastName.compare("-----------------------------------------------------------------------------------------------------")==0){
            	numberOfStudents--;
                continue;
            }
        	fflush(stdin);
            getline(ifInputStudent,firstName);
            fflush(stdin);
            getline(ifInputStudent,id);
            fflush(stdin);
            getline(ifInputStudent,cclass);
            fflush(stdin);
            getline(ifInputStudent,phoneNumber);
            fflush(stdin);
            getline(ifInputStudent,email);
            fflush(stdin);
            getline(ifInputStudent,idCardNumber);
            fflush(stdin);
            getline(ifInputStudent,bankAccountNumber);
            fflush(stdin);
            getline(ifInputStudent,homeTown);
            fflush(stdin);
            getline(ifInputStudent,address);
            ifInputStudent>>numberOfSubjects;
            scores=new float [numberOfSubjects];
            for(int a=0;a<numberOfSubjects;a++){
                ifInputStudent>>scores[a];
            }
            ifInputStudent.ignore();
            student tmp(lastName, firstName, id, cclass, phoneNumber, email, idCardNumber, bankAccountNumber, homeTown, address, numberOfSubjects, scores);
            students[i]=tmp;
            i++;
        }
        sortName(students,numberOfStudents);
    }
}


void output(ofstream& ofInputStudent,student* students,size_t numberOfStudents){
	if(!ofInputStudent.is_open()){
		cout<<"File doesn't exist!"<<endl;
	}else{
		for(int i=0;i<numberOfStudents;i++){
			ofInputStudent<<"-----------------------------------------------------------------------------------------------------"<<endl;
			ofInputStudent<<students[i].getLastName()<<endl;
			ofInputStudent<<students[i].getFirstName()<<endl;
			ofInputStudent<<students[i].getID()<<endl;
			ofInputStudent<<students[i].getClass()<<endl;
			ofInputStudent<<students[i].getPhoneNumber()<<endl;
			ofInputStudent<<students[i].getEmail()<<endl;
			ofInputStudent<<students[i].getIDCardNumber()<<endl;
			ofInputStudent<<students[i].getBankAccountNumber()<<endl;
			ofInputStudent<<students[i].getHomeTown()<<endl;
			ofInputStudent<<students[i].getAddress()<<endl;
			ofInputStudent<<students[i].getNumberOfSubjects()<<endl;
			float *ptr=students[i].getScores();
			for (int a=0;a<students[i].getNumberOfSubjects();a++){
				if(i==numberOfStudents-1 && a==students[i].getNumberOfSubjects()-1){
					ofInputStudent<<ptr[a];
				}else{
					ofInputStudent<<ptr[a]<<endl;
				}
			}
		}
	}
}


void outputStudent(ofstream& ofStudent,student* students,size_t numberOfStudents){
	if(!ofStudent.is_open()){
		cout<<"File doesn't exist!"<<endl;
	}else{
		ofStudent<<setfill(' ')<<setw(8)<<left<<"|STT"<<setw(32)<<left<<"|Ho va ten"<<setw(16)<<left<<"|ID"<<setw(16)<<left<<"|Lop"<<setw(16)<<left<<"|So dien thoai"<<setw(32)<<left<<"|Email"<<setw(24)<<left<<"|So CMND"<<setw(24)<<left<<"|So TK ngan hang"<<setw(48)<<left<<"|Que Quan"<<setw(48)<<left<<"|Dia chi"<<endl;
		ofStudent<<"--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------"<<endl;
		for(int i=0;i<numberOfStudents;i++){
			ofStudent<<"|"<<setfill(' ')<<setw(7)<<left<<i+1;
			ofStudent<<"|"<<setw(31)<<left<<students[i].getFullName();
			ofStudent<<"|"<<setw(15)<<left<<students[i].getID();
			ofStudent<<"|"<<setw(15)<<left<<students[i].getClass();
			ofStudent<<"|"<<setw(15)<<left<<students[i].getPhoneNumber();
			ofStudent<<"|"<<setw(31)<<left<<students[i].getEmail();
			ofStudent<<"|"<<setw(23)<<left<<students[i].getIDCardNumber();
			ofStudent<<"|"<<setw(23)<<left<<students[i].getBankAccountNumber();
			ofStudent<<"|"<<setw(47)<<left<<students[i].getHomeTown();
			ofStudent<<"|"<<setw(47)<<left<<students[i].getAddress()<<endl;
		}
	}
}	


void outputTranscript(ofstream& ofTranscript,student* students,size_t numberOfStudents){
	if(!ofTranscript.is_open()){
		cout<<"File doesn't exist!"<<endl;
	}else{
		for(int i=0;i<numberOfStudents;i++){
			ofTranscript<<"--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------"<<endl;
			float* ptr=students[i].getScores();
			ofTranscript<<endl;
			ofTranscript<<"|Ho va ten: "<<setw(31)<<left<<students[i].getFullName();
			ofTranscript<<"|ID: "<<setw(15)<<left<<students[i].getID();
			ofTranscript<<"|Lop: "<<setw(15)<<left<<students[i].getClass()<<endl;
			ofTranscript<<endl;
			ofTranscript<<"|So mon: "<<students[i].getNumberOfSubjects()<<endl;
			ofTranscript<<endl;
			ofTranscript<<setw(10)<<left<<"|Diem:";
			for (int a=0;a<students[i].getNumberOfSubjects();a++){
					ofTranscript<<setw(5)<<left<<ptr[a];
			}
			ofTranscript<<endl;
			ofTranscript<<endl<<"|Diem trung binh: "<<setprecision(3)<<students[i].averageScore()<<endl;
			ofTranscript<<endl<<"|Xep loai: "<<students[i].rate()<<endl;
			ofTranscript<<endl;
		}
	}
}	


void searchID(student* students,size_t numberOfStudents);


void searchName(student* students,size_t numberOfStudents);


void addStudent(student students[],size_t& numberOfStudents);


void deleteStudent(student students[],size_t& numberOfStudents);


void updateStudent(student students[],size_t& numberOfStudents);


void sortAverageScore(student* students,size_t numberOfStudents);


void displayStudent(student* students,size_t numberOfStudents);


void menu(student students[],size_t& numberOfStudents);


int main(){
    size_t numberOfStudents=0;
	student *students=new student [100000];
    ifstream ifInputStudent("C:/Users/HP/Documents/input.txt",ios::in);
	input(ifInputStudent,students,numberOfStudents);
    ifInputStudent.close();
    menu(students,numberOfStudents);
    ofstream ofInputStudent("C:/Users/HP/Documents/input.txt",ios::out);
    output(ofInputStudent,students,numberOfStudents);
    ofInputStudent.close();
    ofstream ofStudent("C:/Users/HP/Documents/Student.txt",ios::out);
    outputStudent(ofStudent,students,numberOfStudents);
    ofStudent.close();
    ofstream ofTranscript("C:/Users/HP/Documents/Transcript.txt",ios::out);
    outputTranscript(ofTranscript,students,numberOfStudents);
    ofTranscript.close();
    return 0;
}


void displayStudent(student* students,size_t numberOfStudents){
	system("cls");
	cout<<setfill(' ')<<setw(8)<<left<<"|STT"<<setw(32)<<left<<"|Ho va ten"<<setw(16)<<left<<"|ID"<<setw(16)<<left<<"|Lop"<<endl;
	cout<<setfill('-')<<setw(72)<<""<<endl;
	for(int i=0;i<numberOfStudents;i++){
		cout<<"|"<<setfill(' ')<<setw(7)<<left<<i+1;
		cout<<"|"<<setw(31)<<left<<students[i].getFullName();
		cout<<"|"<<setw(15)<<left<<students[i].getID();
		cout<<"|"<<setw(15)<<left<<students[i].getClass()<<endl;
	}
	system("pause");
	system("cls");
	menu(students,numberOfStudents);;
}	


void sortAverageScore(student* students,size_t numberOfStudents){
	system("cls");
	for (int i=0;i<numberOfStudents-1;i++){
		for (int j=i+1;j<numberOfStudents;j++){
			if(students[j].averageScore()>students[i].averageScore()){
				swap(students[i],students[j]);
			}
		}
	}
	cout<<setfill(' ')<<setw(8)<<left<<"|STT"<<setw(32)<<left<<"|Ho va ten"<<setw(16)<<left<<"|ID"<<setw(16)<<left<<"|Lop"<<setw(20)<<left<<"|Diem trung binh"<<setw(16)<<left<<"|Xep loai"<<endl;
	cout<<setfill('-')<<setw(108)<<""<<endl;
	for(int i=0;i<numberOfStudents;i++){
		cout<<"|"<<setfill(' ')<<setw(7)<<left<<i+1;
		cout<<"|"<<setw(31)<<left<<students[i].getFullName();
		cout<<"|"<<setw(15)<<left<<students[i].getID();
		cout<<"|"<<setw(15)<<left<<students[i].getClass();
		cout<<"|"<<setw(19)<<left<<setprecision(3)<<students[i].averageScore();
		cout<<"|"<<setw(15)<<left<<students[i].rate()<<endl;
	}
	system("pause");
	system("cls");
	menu(students,numberOfStudents);
	sortName(students,numberOfStudents);
}


void searchID(student students[],size_t numberOfStudents){
	system("cls");
	sortID(students,numberOfStudents);
	string lookFor;
		cout<<"Nhap ID muon tim kiem hoac bam 0 de thoat: ";
		cin>>lookFor;
		if(lookFor.compare("0")==0){
			system("cls");
			menu(students,numberOfStudents);
		}else{
			size_t position;
			bool found=false;
		    int first=0;
		    int last=numberOfStudents-1;
		    while(first<=last){
		        int mid=(first+last)/2;
		        if(students[mid].getID().compare(lookFor)==0){
		        found=true;
		        position=mid;
		        break;
		        }
		        else if(students[mid].getID().compare(lookFor)>0){
		        last = mid - 1;
		        }
		        else if(students[mid].getID().compare(lookFor)<0){
		        first = mid + 1;
		        }
		    }
		    if(found==false){
		    	cout<<"ID nay khong ton tai, hay thu lai!"<<endl;
		    	system("pause");
		    	searchID(students,numberOfStudents);
			}else{
				system("cls");
				students[position].display();
			}
		}
	sortName(students,numberOfStudents);
	system("pause");
	system("cls");
	menu(students,numberOfStudents);
}


void searchName(student students[],size_t numberOfStudents){
	system("cls");
	sortFullName(students,numberOfStudents);
	string lookFor;
		cout<<"Nhap ten muon tim kiem hoac bam 0 de thoat: ";
		fflush(stdin);
		getline(cin,lookFor);
		if(lookFor.compare("0")==0){
			system("cls");
			menu(students,numberOfStudents);
		}else{
			size_t *position=new size_t [100];
			bool found=false;
			int numberFound=0;
		    int first=0;
		    int last=numberOfStudents-1;
		    while(first<=last){
		        int mid=(first+last)/2;
		        if(students[mid].getFullName().compare(lookFor)==0){
		        	found=true;
		        	int a=0;
		        	for(int i=first;i<last;i++){
		        		if(students[i].getFullName().compare(lookFor)==0){
		        			numberFound++;
		        			position[a]=i;
		        			a++;
						}
					}
		        break;
		        }
		        else if(students[mid].getFullName().compare(lookFor)>0){
		        last = mid - 1;
		        }
		        else if(students[mid].getFullName().compare(lookFor)<0){
		        first = mid + 1;
		        }
		    }
		    if(found==false){
		    	cout<<"Nguoi nay khong ton tai, hay thu lai!"<<endl;
		    	system("pause");
		    	searchName(students,numberOfStudents);
			}else{
				system("cls");
				cout<<"Tim duoc "<<numberFound<<" ket qua: "<<endl;
				for(int i=0;i<numberFound;i++){
					students[position[i]].display();
				}
			}
		}
	sortName(students,numberOfStudents);
	system("pause");
	system("cls");
	menu(students,numberOfStudents);
}


void addStudent(student students[],size_t& numberOfStudents){
	system("cls");
	string  lastName,
    		firstName,
            id,
            cclass,
            phoneNumber,
            email,
            idCardNumber,
            bankAccountNumber,
            homeTown,
            address;
    size_t numberOfSubjects;
    float *scores;
    cout<<"Nhap ID hoac nhap 0 de thoat: ";
    fflush(stdin);
    getline(cin,id);
    if(id.compare("0")==0){
    	system("cls");
    	menu(students,numberOfStudents);
	}else{
		sortID(students,numberOfStudents);
		bool found=false;
	    int first=0;
		int last=numberOfStudents-1;
		while(first<=last){
		    int mid=(first+last)/2;
		       if(students[mid].getID().compare(id)==0){
			       found=true;
			       break;
		        }
		       	else if(students[mid].getID().compare(id)>0){
			    	last = mid - 1;
			    }
			    else if(students[mid].getID().compare(id)<0){
			       first = mid + 1;
			    }
		    }
		if(found==true){
			cout<<"ID nay da ton tai, hay thu lai!"<<endl;
			system("pause");
			addStudent(students,numberOfStudents);
		}else{
			cout<<"Nhap ho va ten dem: ";
			fflush(stdin);
			getline(cin,lastName);
		    cout<<"Nhap ten: ";
			fflush(stdin);
			getline(cin,firstName);
			cout<<"Lop: ";
			fflush(stdin);
			getline(cin,cclass);
			cout<<"So dien thoai: ";
			fflush(stdin);
			getline(cin,phoneNumber);
			cout<<"Email: ";
			fflush(stdin);
			getline(cin,email);
			cout<<"So cmnd/the cccd: ";
			fflush(stdin);
			getline(cin,idCardNumber);
			cout<<"So tai khoan ngan hang: ";
			fflush(stdin);
			getline(cin,bankAccountNumber);
			cout<<"Que quan: ";
			fflush(stdin);
			getline(cin,homeTown);
			cout<<"Dia chi: ";
			fflush(stdin);
			getline(cin,address);
			cout<<"So mon: ";
			cin>>numberOfSubjects;
			scores=new float [numberOfSubjects];
			for(int a=0;a<numberOfSubjects;a++){
				cout<<"Nhap diem mon "<<a+1<<": ";
	    		cin>>scores[a];
			}
			student tmp(lastName, firstName, id, cclass, phoneNumber, email, idCardNumber, bankAccountNumber, homeTown, address, numberOfSubjects, scores);
			students[numberOfStudents]=tmp;
			system("cls");
			cout<<setfill('-')<<setw(50)<<""<<endl;
			cout<<setfill(' ')<<setw(35)<<"Da them thanh cong!"<<endl;
			students[numberOfStudents].display();
			cout<<setfill('-')<<setw(50)<<""<<endl;
			numberOfStudents++;
			sortName(students,numberOfStudents);
			system("pause");
			system("cls");
			menu(students,numberOfStudents);
		}
	}
}


void deleteStudent(student students[],size_t &numberOfStudents){
	system("cls");
	sortID(students,numberOfStudents);
	fflush(stdin);
	string lookFor;
		cout<<"Nhap ID muon xoa hoac bam 0 de thoat: ";
		getline(cin,lookFor);
		if(lookFor.compare("0")==0){
			system("cls");
			menu(students,numberOfStudents);
		}else{
			size_t position;
			bool found=false;
		    int first=0;
		    int last=numberOfStudents-1;
		    while(first<=last){
		        int mid=(first+last)/2;
		        if(students[mid].getID().compare(lookFor)==0){
		        found=true;
		        position=mid;
		        break;
		        }
		        else if(students[mid].getID().compare(lookFor)>0){
		        last = mid - 1;
		        }
		        else if(students[mid].getID().compare(lookFor)<0){
		        first = mid + 1;
		        }
		    }
		    if(found==false){
		    	cout<<"ID nay khong ton tai, hay thu lai!"<<endl;
		    	system("pause");
		    	deleteStudent(students,numberOfStudents);
			}else{
				for(int i=position;i<numberOfStudents;i++){
					swap(students[i],students[i+1]);
				}
				numberOfStudents--;
				cout<<"-------------------------------------------"<<endl;
				cout<<setfill(' ')<<setw(30)<<"Da xoa thanh cong!"<<endl;
				cout<<"-------------------------------------------"<<endl;
			}
		}
	sortName(students,numberOfStudents);
	system("pause");
	system("cls");
	menu(students,numberOfStudents);
}


void updateStudent(student students[],size_t& numberOfStudents){
	system("cls");
	sortID(students,numberOfStudents);
	string lookFor;
	cout<<"Nhap ID muon chinh sua hoac bam 0 de thoat: ";
	fflush(stdin);
	getline(cin,lookFor);
	if(lookFor.compare("0")==0){
		system("cls");
		menu(students,numberOfStudents);
	}else{
		size_t position;
		bool found=false;
		int first=0;
		int last=numberOfStudents-1;
		while(first<=last){
		    int mid=(first+last)/2;
		    if(students[mid].getID().compare(lookFor)==0){
		    	found=true;
		    	position=mid;
		    	break;
		    }
		    else if(students[mid].getID().compare(lookFor)>0){
		        last = mid - 1;
		    }
		    else if(students[mid].getID().compare(lookFor)<0){
		    	first = mid + 1;
		    }
		}
		if(found==false){
			cout<<"ID nay khong ton tai, hay thu lai!"<<endl;
		    updateStudent(students,numberOfStudents);
		}else{
			while(true){
				system("cls");
				cout<<"****************************************"<<endl;
				cout<<"**  1.Sua ten.                        **"<<endl;
				cout<<"**  2.Sua ID.                         **"<<endl;
				cout<<"**  3.Sua lop.                        **"<<endl;
				cout<<"**  4.Sua so dien thoai.              **"<<endl;
				cout<<"**  5.Sua email.                      **"<<endl;
				cout<<"**  6.Sua so chung minh nhan dan.     **"<<endl;
				cout<<"**  7.Sua so tai khoang ngan hang.    **"<<endl;
				cout<<"**  8.Sua que quan.                   **"<<endl;
				cout<<"**  9.Sua dia chi.                    **"<<endl;
				cout<<"**  10.Them diem.                     **"<<endl;
				cout<<"**  0.Thoat.                          **"<<endl;
				cout<<"****************************************"<<endl;
				cout<<"Chon thao tac: ";
				int x;
				cin>>x;
				system("cls");
				switch (x){
					case 1:{
						system("cls");
						string lastName;
						string firstName;
						fflush(stdin);
						cout<<"Nhap ho va ten dem: ";
						getline(cin,lastName);
						fflush(stdin);
						cout<<"Nhap ten: ";
						cin>>firstName;
						students[position].setName(lastName,firstName);
						system("cls");
						cout<<endl;
						cout<<setfill('-')<<setw(50)<<""<<endl;
						cout<<setfill(' ')<<setw(35)<<"Da sua thanh cong!"<<endl;
						students[position].display();
						cout<<setfill('-')<<setw(50)<<""<<endl;
						system("pause");
						break;
					}
					case 2:{
						string id;
						cout<<"Nhap ID: ";
						fflush(stdin);
						cin>>id;
						students[position].setID(id);
						system("cls");
						cout<<endl;
						cout<<setfill('-')<<setw(50)<<""<<endl;
						cout<<setfill(' ')<<setw(35)<<"Da sua thanh cong!"<<endl;
						students[position].display();
						cout<<setfill('-')<<setw(50)<<""<<endl;
						system("pause");
						break;
					}
					case 3:{
						string cclass;
						cout<<"Nhap lop: ";
						fflush(stdin);
						cin>>cclass;
						students[position].setClass(cclass);
						system("cls");
						cout<<endl;
						cout<<setfill('-')<<setw(50)<<""<<endl;
						cout<<setfill(' ')<<setw(35)<<"Da sua thanh cong!"<<endl;
						students[position].display();
						cout<<setfill('-')<<setw(50)<<""<<endl;
						system("pause");
						break;
					}
					case 4:{
						string phoneNumber;
						cout<<"Nhap so dien thoai: ";
						fflush(stdin);
						cin>>phoneNumber;
						students[position].setPhoneNumber(phoneNumber);
						system("cls");
						cout<<endl;
						cout<<setfill('-')<<setw(50)<<""<<endl;
						cout<<setfill(' ')<<setw(35)<<"Da sua thanh cong!"<<endl;
						students[position].display();
						cout<<setfill('-')<<setw(50)<<""<<endl;
						system("pause");
						break;
					}
					case 5:{
						string email;
						cout<<"Nhap email: ";
						fflush(stdin);
						cin>>email;
						students[position].setEmail(email);
						system("cls");
						cout<<endl;
						cout<<setfill('-')<<setw(50)<<""<<endl;
						cout<<setfill(' ')<<setw(35)<<"Da sua thanh cong!"<<endl;
						students[position].display();
						cout<<setfill('-')<<setw(50)<<""<<endl;
						system("pause");
						break;
					}
					case 6:{
						string idCardNumber;
						cout<<"Nhap so chung minh nhan dan: ";
						fflush(stdin);
						cin>>idCardNumber;
						students[position].setIDCardNumber(idCardNumber);
						system("cls");
						cout<<endl;
						cout<<setfill('-')<<setw(50)<<""<<endl;
						cout<<setfill(' ')<<setw(35)<<"Da sua thanh cong!"<<endl;
						students[position].display();
						cout<<setfill('-')<<setw(50)<<""<<endl;
						system("pause");
						break;
					}
					case 7:{
						string bankAccountNumber;
						cout<<"Nhap so tai khoang ngan hang: ";
						fflush(stdin);
						cin>>bankAccountNumber;
						students[position].setBankAccountNumber(bankAccountNumber);
						system("cls");
						cout<<endl;
						cout<<setfill('-')<<setw(50)<<""<<endl;
						cout<<setfill(' ')<<setw(35)<<"Da sua thanh cong!"<<endl;
						students[position].display();
						cout<<setfill('-')<<setw(50)<<""<<endl;
						system("pause");
						break;
					}
					case 8:{
						string homeTown;
						cout<<"Nhap que quan: ";
						fflush(stdin);
						getline(cin,homeTown);
						students[position].setHomeTown(homeTown);
						system("cls");
						cout<<endl;
						cout<<setfill('-')<<setw(50)<<""<<endl;
						cout<<setfill(' ')<<setw(35)<<"Da sua thanh cong!"<<endl;
						students[position].display();
						cout<<setfill('-')<<setw(50)<<""<<endl;
						system("pause");
						break;
					}
					case 9:{
						string address;
						cout<<"Nhap dia chi: ";
						fflush(stdin);
						getline(cin,address);
						students[position].setAddress(address);
						system("cls");
						cout<<endl;
						cout<<setfill('-')<<setw(50)<<""<<endl;
						cout<<setfill(' ')<<setw(35)<<"Da sua thanh cong!"<<endl;
						students[position].display();
						cout<<setfill('-')<<setw(50)<<""<<endl;
						system("pause");
						break;
					}	
					case 10:{
						size_t addNumberOfSubjects;
						float *scores;
						cout<<"Nhap so mon them vao: ";
						cin>>addNumberOfSubjects;
						scores=new float[addNumberOfSubjects];
						cout<<"Nhap diem: ";
						for(int i=0;i<addNumberOfSubjects;i++){
							cin>>scores[i];
						}
						students[position].setScores(scores,addNumberOfSubjects);
						system("cls");
						cout<<endl;
						cout<<setfill('-')<<setw(50)<<""<<endl;
						cout<<setfill(' ')<<setw(35)<<"Da sua thanh cong!"<<endl;
						students[position].display();
						cout<<setfill('-')<<setw(50)<<""<<endl;
						system("pause");
						break;
					}
					case 0:{
						system("cls");
						break;
					}
				}
				if(x==0){
					break;
				}
			}
		}
	sortName(students,numberOfStudents);
	system("cls");
	menu(students,numberOfStudents);
	}
}
void menu(student students[],size_t& numberOfStudents){
	cout << "CHUONG TRINH QUAN LY SINH VIEN C/C++ - NHOM HMB"<<endl;
    cout << "*************************MENU**************************"<<endl;
    cout << "**  1. Them sinh vien.                               **"<<endl;
    cout << "**  2. Cap nhat thong tin sinh vien boi ID.          **"<<endl;
    cout << "**  3. Xoa sinh vien boi ID.                         **"<<endl;
    cout << "**  4. Tim kiem sinh vien theo ten.                  **"<<endl;
    cout << "**  5. Tim kiem sinh vien theo id.                   **"<<endl;
    cout << "**  6. Sap xep sinh vien theo diem trung binh (GPA). **"<<endl;
    cout << "**  7. Hien thi danh sach sinh vien.                 **"<<endl;
    cout << "**  0. Thoat                                         **"<<endl;
    cout << "*******************************************************"<<endl;
    cout << "Nhap tuy chon: ";
    int key;
    cin >> key;
    switch (key){
    	case 1:
    		addStudent(students,numberOfStudents);
    		break;
    	case 2:
    		updateStudent(students,numberOfStudents);
    		break;
    	case 3:
    		deleteStudent(students,numberOfStudents);
    		break;
    	case 4:
    		searchName(students,numberOfStudents);
    		break;
    	case 5:
    		searchID(students,numberOfStudents);
    		break;
    	case 6:
    		sortAverageScore(students,numberOfStudents);
    		break;
    	case 7:
    		displayStudent(students,numberOfStudents);
    		break;
    	case 8:
    		break;
	}
}
