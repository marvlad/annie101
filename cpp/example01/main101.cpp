#include <iostream>

using namespace std;

void test1(){
	cout << "test1" << endl;
}

int test2(int a, int &b){
	int c = 5;
	b = c*c;
	return a + c;
}

class experiment{
	public:
		string name;
		// constructor
		experiment(string n){
			name = n;
		}

		void sayHello(){
			cout << "Hello, experiment's name: " << name << endl;
		}
	
		int collaborators = 1;
	private:
		double age = 9.4;
	
};

class annie: public experiment{
	public:
		int nlappds =  100;
		annie(string name):experiment(name){}
		friend class dune;
	protected:
		int pot = 10;
};

struct collaborator {
	string name;
	string role;
	collaborator(std::string n, std::string r) : name(n), role(r) {}
};

class dune: public experiment{
	public:
		collaborator coll;
		dune(string name, collaborator c):experiment(name),coll(c){}
		virtual void sayHello(){
			cout << "Hello, experiment's name: " << name << endl;
			cout << "Collaborators: " << coll.name << " " << coll.role << endl;
		}
		void inspectAnnie(annie& a) {
			cout << " == Inspecting ANNIE from DUNE --" << endl;
			cout << "ANNIE POT (remember this is protected memeber): " << a.pot <<  endl;
		}
};

template <typename T> T comapre(T x, T y){
	return (x >  y) ? x : y;
}

int main(){
	// printf("Hello ANNIE %d \n", 1);

	using namespace std;

	//cout << "ANNIE";
	// what is "ANNIE"? --> char*
	// "<<" print out as side effect

	// int a = 2;
	// cout << a << endl;
	// int b = 1;
	// int c = (a << b); // a=10 (base 2), 10->100 one bit left
	// cout << c;
	// a << b; //

	// int x = 4;
	// int y = 5;
	// x += y;
	// cout << x;
	// cout << y;
	// int z = (x += y); //? Important!

	// r = cout << z; // what is r?
	// ostream& o = cout << x;//
	// o << "SuperANNIE\n";	

	// test1();
	// int x = 2;
	// int y;

	// int a = test2(x,y);
	// cout << a << endl;
	// cout << x << endl;
	// cout << y << endl;
	
	// experiment a("ANNIE");
	// a.sayHello();
	// cout << a.collaborators << endl; //?
	// cout << a.age << endl; //?

	annie a("annieII");
	a.sayHello();
	cout << a.nlappds << endl;

	collaborator c("Fermi","student");
	dune d("DUNE_pahseI",c);
	d.sayHello();
	d.inspectAnnie(a);
	// cout << compare<int>(4,6) << endl;
	// cout << compare<double>(4.3, 6.4) << endl;
	// compare<char>('l','s') << endl; // why is s?

	return 0;
}
