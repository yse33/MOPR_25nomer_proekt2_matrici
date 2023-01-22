#include "matrix.h"

Matrix::Matrix(const string& file_name) {
    //file-ut se otvarq i se proverqva dali se e otvoril uspeshno
    ifstream matrix_file(file_name);
    if (!matrix_file.is_open()) {
        throw runtime_error("Could not open file!");
    }
    //vuv file-a ima 5 reda i 4 koloni, obache e po-lesno da se izpolzva 4x5 matrica
    //za purvite 4 reda ot file-a vsichko e normalno, kato se slaga na matrix[rows][1-4]
    //posledniq red obache se tretira razlichno
    //da kajem che toi ima sledniq vid: 1 2 3 4
    //1 shte otide kato posledna kolona na purvi red, 2 shte otida kato posledna kolona na vtori red
    //i taka dokato ne se napulni matricata
    for (int rows = 0; rows < 5; rows++) {
        for (int columns = 0; columns < 4; columns++) {
            if (rows != 4) {
                matrix_file >> matrix[rows][columns];
            } else {
                matrix_file >> matrix[columns][4];
            }
        }
    }
    //vajno e da se zatvarq file-a vseki put pri otvarqne
    matrix_file.close();

    //pulnqt se trite vector<double>-a na class Matrix - bills(na ruka), actual_bills i matrix_roots - s metodi, opisani nadolu
    bills = {0.129, 1.461, 85.07, 0.132};
    fill_actual_bills();
    fill_matrix_roots();
}

//copy constructor, koito kopira samo matricata, bez vectorite, zashtoto ne e nujno, za celta, za koqto se polzva
Matrix& Matrix::operator=(const Matrix &other) {
    if (this != &other) {
        for (int rows = 0; rows < 4; rows++) {
            for (int columns = 0; columns < 5; columns++) {
                this->matrix[rows][columns] = other.matrix[rows][columns];
            }
        }
    }

    return *this;
}

//metod, koito deli vseki double v reda na divider
void Matrix::divide_row(int row, double divider) {
    for (int i = 0; i < 5; i++) {
        matrix[row][i] /= divider;
    }
}

//metod, koito umnojava vseki double v reda po mutiplier
void Matrix::multiply_row(int row, double multiplier) {
    for (int i = 0; i < 5; i++) {
        matrix[row][i] *= multiplier;
    }
}

//metod, koido umnojava subtraction_row s multiplier kato vika gorniq metod
//sled tova izvajda vseki element na subtraction_row ot tozi na sushtata poziciq v row
//za da se vurne subtraction_row s originalnite si stoinosti go delim na multiplier
void Matrix::subtract_row_from_another(int row, int subtraction_row, double multiplier) {
    multiply_row(subtraction_row, multiplier);

    for (int columns = 0; columns < 5; columns++) {
        matrix[row][columns] -= matrix[subtraction_row][columns];
    }

    divide_row(subtraction_row, multiplier);
}

//metod, koito smenq elementite na 2 reda
void Matrix::switch_rows(int row, int other_row) {
    for (int columns = 0; columns < 5; columns++) {
        double temp = matrix[row][columns];
        matrix[row][columns] = matrix[other_row][columns];
        matrix[other_row][columns] = temp;
    }
}

//metod, koito proverqva dali matricata ima samo 1 reshenie
// Koga se vika sledniq metod?
//pri metoda na Gaus pravq diagonalna edinichna matrica
//tova znachi che delq reda, v koito se namira elementa, na stoinostta my, za da polucha 1
//ako obache tozi element = 0, to 0/0 = 0, a ne 1
//pri takuv sluchai trqbva da se vikne switch_rows, toest da se smeni reda s drug
//obache e vuzmojno matricata da ima 0 ili poveche ot 1 reshenie
//pri takiva sluchai stoinostite na elementite v sushtata kolona, no po-dolni redove na elementa vsichki shte sa 0
//ako tova e taka se throwva exception za nevaliden input
//ako se nameri element v sushtata kolona na po-dolen red, razlichen ot 0, se razmenqt redovete sus switch_rows
void Matrix::check_if_matrix_has_only_1_solution(int given_row) {
    for (int rows = given_row + 1; rows < 4; rows++) {
        if (matrix[rows][given_row] != 0) {
            switch_rows(given_row, rows);
            return;
        }
    }

    throw runtime_error("Matrix should have only 1 solution!");
}

//metod, koito zapulva vector<double< actuall_bills
//za vsqka kompaniq se vzima element ot matrix s indexi [rows][columns] i se umnojava po element ot bills s index columns
//bills e zapulnen taka, che element 1 otgovarq na smetkata za purvata kolona ot input file-a, element 2 za vtorata i t.n.
//suotvetno kogato se vzeme element ot rows columns naprimer rows = 0 i columns = 1
//razglejda se vtorata smetka na purvata kompaniq, toest se umnojava po stoinostta na vtorata smetka ot bills
//proizvedenieto se subira s actual_bill_for_company, koeto priema stoinost 0 vseki put kogato se smeni reda
//toest za vseki red se vzima sbora ot proizvedeniqta, kato se iterirae samo do 4tata kolona, zashtoto 5tata e nerelevantna
//predi da rows da se smeni i da se izgubi sborut, toi se zapisva vuv vector actual_bills
void Matrix::fill_actual_bills() {
    for (int rows = 0; rows < 4; rows++) {

        double actual_bill_for_company = 0;
        for (int columns = 0; columns < 4; columns++) {
            actual_bill_for_company += matrix[rows][columns] * bills.at(columns);
        }

        actual_bills.push_back(actual_bill_for_company);
    }
}

//metod, koito zapulva vector<double> matrix_roots, toest korenite na matricata ot file-a
//metodut izpozlva metoda na Gaus, kato 4-te korena shte sa realno stoinostite na poslednite elementi vuv vseki red sled-
//-vsichki izvursheni ot metoda operacii
//trqbva da se stigne do 4x4 matrica. za da se postigne tova e nujno da se izbere edin element ot vsqka kolona
//tozi element trqbva da stane 1, posle vsichki drugi da stanat 0
//v implementaciqta ma metoda se izbirat purviqt element ot purvata kolona, vtoriqt element ot vtorata i t.n.
//toest nakraq edinicite shte obrazuvat diagonal ot gore lqvo do dolu dqsno v matricata
//poneje v implementaciqta se poluchava diagonal, to edinicite shte sa na indexi [rows][rows] v matrix - (1,1),(2,2)...(4,4)
//vseki put kogato se smeni counterut vuv purviq for - toest kogato se incrementira stoinostta na rows
//trqbva da se suzdade purvo edinicata, posle da se napravqt nulite
//edinicata se suzdava po sledniq nachin : reda se deli na stoinostta na chisloto, koeto trqbva da stane 1, zashtoto
//vsqko chislo, razdeleno na sebe si, e 1
//sled tova ot vseki element v tazi kolona, osven tozi, koito e 1, trqbva da se izvadi 1 * sebe si, taka che da stane 0
//if-ut podsigurqva, che ne izvajdame reda s edinica ot sebe si, zashtoto tova shte mahne edinicata
//da kajem che rows = 0, toest element (1,1) ot matricata trqbva da e 1;
//deli se na sebe si, sled koeto vuv vtoriq for se propuska purvata iteraciq, zashtoto columns == rows
//sled tova za vseki ostanal se izvajda 1 * stoinostta na elementa v kolonata, toest x -= 1 * x, koeto rezultira v 0
//taka matricata se svejda do edinichna, kato stoinostite na poslednite elementi vuv vseki red sa korenite
//NB: izpolzva se kopie na matricata, zashtoto inache shte se promenqt stoinostite na elementite v neq
void Matrix::fill_matrix_roots() {
    Matrix matrix_for_roots = *this;

    for (int rows = 0; rows < 4; rows++) {
        if (matrix[rows][rows] == 0) {
            check_if_matrix_has_only_1_solution(rows);
        } else {
            matrix_for_roots.divide_row(rows, matrix_for_roots.matrix[rows][rows]);
        }
        for (int columns = 0; columns < 4; columns++) {
            if (columns != rows) {
                matrix_for_roots.subtract_row_from_another(
                        columns, rows, matrix_for_roots.matrix[columns][rows]
                );
            }
        }

    }

    for (int i = 0; i < 4; i++) {
        roots.push_back(matrix_for_roots.matrix[i][4]);
    }
}

//neizpolzvan metod, koito printira matrica
void Matrix::print_matrix() {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 5; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }

    cout << endl;
}

//metod, koito printira nujniq v zadanieto output
//purvo printira razlikata mejdu obshtata smetka na cqlata kompaniq i realnata i smetka
//sled tova se vzima vseki element, koito realno predstavlqva broq na uslugite
//toi se umnojava se po realno taksuvanata cena(realno taksuvanite ceni sa korenite na matricata)
//zashtoto: matricata realno e sistema ot 4 uravneniq(shte izpolzvam purviq primeren output za demonstraciq):
//50*a + 7*b + 2*c + 130*d = 250
//45*a + 8*b + 3*c + 200*d = 365 i t.n.
//realno a, b, c i d sa realno taksuvanite ceni za vsqko edinica ot broq na uslugite
//toest kato se namerqt korenite se namirat realno taksuvanite ceni
//proizvedenieto na dadeniq element s realno taksuvanata cena se deli na cenata ot internet
//ot cqloto tova se izvajda broq na uslugite
//tova cqloto se printira, kato se izpolzva tazi formula, zashtoto taka e kazano v zadanieto
void Matrix::print_result() {
    for(int i = 0; i < 4; i++) {
        cout << (matrix[i][4] - actual_bills.at(i)) << " ";
    }

    for (int i = 0; i < 4; i++) {
        cout << endl;
        for (int j = 0; j < 4; j++) {
            cout << ((matrix[i][j] * roots.at(j) / bills.at(j)) - matrix[i][j]) << " ";
        }
    }
}
