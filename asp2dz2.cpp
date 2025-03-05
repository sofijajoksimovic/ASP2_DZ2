#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <queue>
#include <string>
#include <iomanip>
using namespace std;
struct Zapis;
//struktura cvora B stabla
struct Node{
    int n;//broj kljuceva u cvoru
    Node**podstabla;//pokazivac na niz na podstabla
    Node*parent;//pokazivac na oca
    Zapis**kljucevi;//niz pokazivaca na kljuceve koji su sami po sebi lista, ulancavaju se ako imaju isti idPrimary
    bool isList;
    int minBrKljucevaUListu;
    int minBrPodstabala;
    Node(int n, int red, bool isList, Node* parent= nullptr):n(n),isList(isList),parent(parent){
        podstabla=new Node*[red+1];//red+1 podstablo;najvise red ali u slucaju prebacivanja
        for(int i=0;i<red+1;i++)
        {
            podstabla[i]= nullptr;
        }
        kljucevi=new Zapis * [red];//red zapisa ;najvise red-1 kljuceva tj zapisa
        for(int i=0;i<red;i++)
        {
            kljucevi[i]= nullptr;
        }
        minBrPodstabala=int(ceil((double)red/2));
        minBrKljucevaUListu=int(ceil((double)red/2))-1;//br kljuceva uvek za jedan manni od broja cvorova
    }
};
//struktura zapis, pravimo listu za svaki kljuc u cvoru od zapisa
struct Zapis{
    long long int idCustomer;
    long long int idPrimary;
    long long int idSecondary;
    string name;
    int tax;
    long double bal;
    Zapis*next;
    Zapis()=default;
    Zapis(long long int idCustomer, long long int idPrimary, long long int idSecondary, string& name, int tax, long double bal):
            idCustomer(idCustomer),idPrimary(idPrimary),idSecondary(idSecondary),name(name),tax(tax),bal(bal),next(nullptr){
    }
};
//struktura Customer, od podataka o Customeru od nje na poctku napravimo listu da bi je kasnije kosritili za pretragu
struct Customer{
    long long int cId;
    string cfname;
    string clname;
    string email;
    long long int caId;
    Customer*next;
    Customer(long long int cId, string& cfname, string& clname, string& email, long long int caId):cId(cId),
    cfname(cfname),clname(clname),email(email),caId(caId),next(nullptr){
    }
};
//pretraga koja vraca cvor u kome je zapis ili u kome bi trebalo da bude ako ga nema, i eventualno broj koraka pretrage
Node* pretraziStabloNaZapis(Node*root,Zapis*zapis, int red, int& br){
    if(root->n==0)return root;
    Node* temp=root;
    while(temp!= nullptr) {
        br++;
        int i=0;
        while(i<temp->n and temp->kljucevi[i]->idCustomer<zapis->idCustomer){
            i++;
        }
        if(i==temp->n){
            if(temp->isList)return temp;
            else
            {
                temp = temp->podstabla[i];
            }
        }
        else if(temp->kljucevi[i]->idCustomer==zapis->idCustomer){
            return temp;
        }
        else{
            if(temp->isList)return temp;
            else
            {
                temp = temp->podstabla[i];
            }
        }
    }
    return nullptr;
}
Zapis** sortirajCvor(Node*cvor, Zapis*zapis, int red){
    Zapis **niz=new Zapis * [red];
    int j=0;
    while(j<cvor->n and cvor->kljucevi[j] and cvor->kljucevi[j]->idCustomer<zapis->idCustomer){
        niz[j]=cvor->kljucevi[j];
        j++;
    }

    niz[j++]=zapis;
    while(j-1<cvor->n){
        niz[j]=cvor->kljucevi[j-1];
        j++;
    }
    return niz;
}
//ubacuje novokreirano podstablo u niz podstabala
Zapis** ubaciUCvor(Node* cvor, Zapis* zapis, int red, Node* brat){

    Zapis**rNiz= sortirajCvor(cvor,zapis,red);
    cvor->n++;
    for(int i=0;i<red;i++){
        cvor->kljucevi[i]=rNiz[i];
    }
    int j=0;
    Node* nizPodstabala[red+1];
    while(j<red+1 and cvor->podstabla[j] and cvor->podstabla[j]->kljucevi[0]->idCustomer<brat->kljucevi[0]->idCustomer){
        nizPodstabala[j]=cvor->podstabla[j];
        j++;
    }
    nizPodstabala[j++]=brat;
    while(j<red+1){
        nizPodstabala[j]=cvor->podstabla[j-1];
        j++;
    }
    for(int i=0;i<red+1;i++){

        cvor->podstabla[i]=nizPodstabala[i];
    }
    return  rNiz;

}
//SPLIT funkcija, umetanje u slucaju overflow
Node* split(Node* root, Node* cvor, Zapis* zapis,int red){
    //ako je cvor list, srednji saljemo u parent, ako je parent nullprt, pravimo novi cvor za koren u koji stavljamo samo taj srednji zapis
    //ako nije nullptr proveramo da li je i on pun ako nije tu se operacija zavrsava
    //ako je roditeljski cvor pun onda se operacija propagira na njega
    Zapis**niz= sortirajCvor(cvor,zapis, red);
    cvor->n++;
    for(int i=0;i<cvor->n;i++){
        cvor->kljucevi[i]=niz[i];
    }
    while(cvor->n>=red){
        int mid=ceil((double)red/2);
        Zapis* srednji=niz[mid-1];
        Node* brat=new Node(red/2,red,cvor->isList,cvor->parent);
        for(int i=mid;i<red;i++){
            brat->kljucevi[i-mid]=niz[i];
            cvor->kljucevi[i]= nullptr;
        }
        cvor->kljucevi[mid-1]= nullptr;
        cvor->n=mid-1;
        if(!cvor->isList){
            for(int i=mid;i<red+1;i++){
                brat->podstabla[i-mid]=cvor->podstabla[i];
                cvor->podstabla[i]= nullptr;
            }
            int j=0;
            while(brat->podstabla[j]){
                brat->podstabla[j]->parent=brat;
                j++;
            }
        }
        if(cvor->parent){
            Zapis**niz1=ubaciUCvor(cvor->parent,srednji,red, brat);
            cvor=cvor->parent;
            niz=niz1;
        }
        else
        {
            Node* newRoot=new Node(1,red,false);
            newRoot->kljucevi[0]=srednji;
            newRoot->podstabla[0]=cvor;
            newRoot->podstabla[1]=brat;
            cvor->parent=newRoot;
            brat->parent=newRoot;
            root=newRoot;
            return root;
        }

    }
    delete niz;
    return root;
}
bool postojiID(Node*root, long long int idCust){
    if (root == nullptr) {
        return false;
    }
    std::queue<Node*> q;
    q.push(root);
    while (!q.empty()) {
        Node* current = q.front();
        q.pop();
        Zapis*tmp=current->kljucevi[0];
        while(tmp){
            if(tmp->idPrimary==idCust)
                return true;
            tmp=tmp->next;
        }
        for (int i = 0; i <= current->n and current->podstabla[i] != nullptr; i++) {
            q.push(current->podstabla[i]);
        }
    }
    return false;
}
Node* dodajZapisUStablo(Node*root,Zapis*zapis,int red){
    int br=0;
    Node* temp=pretraziStabloNaZapis(root,zapis, red,br);
    //cvor na koji nas upucuje poretraga odnosno u kome se pretraga zavrsava
    int i=0;
    if(temp== nullptr)return root;
    if(postojiID(root,zapis->idPrimary)){
        cout<<"Greska, vec postoji racun sa zadatim primarnim kljucem!"<<endl;
        return root;
    }
    while(i<temp->n and temp->kljucevi[i]->idCustomer<zapis->idCustomer)
    {
        i++;
    }
    if(temp->kljucevi[i] and temp->kljucevi[i]->idCustomer==zapis->idCustomer)
    {
        //ako vec imamo customera sa nekim racunom u cusacc treba novog da ulancamo u listu ako taj racun ne popstoji
        Zapis*tmp=temp->kljucevi[i];
        while(tmp->next)
        {
            tmp=tmp->next;
        }
        tmp->next=zapis;
        return root;
    }
    //ubaciti u stablo
    //prvo proveriti jel n<red-1 tj ima li mesta ako ima ubacimo samo u sortiranom poretku
    //u suprotnom dolazi do preloma
    else {
        if (temp->n < red - 1)
        {
            temp->n++;
            for(int j=temp->n-1;j>i;j--){
                temp->kljucevi[j]=temp->kljucevi[j-1];
            }
            temp->kljucevi[i]=zapis;
            return root;
        }

        else
        {
            //split cvora
            root=split(root,temp,zapis,red);
            return root;
        }
    }
}
Node* unesiCvoroveIzDatoteke(Node*root,int red, string imeDatoteke){
    ifstream datoteka;
    datoteka.open(imeDatoteke);
    string line;
    while(getline(datoteka,line)){
        istringstream iss(line);
        string idprimary, idsecondary, idcustomer, name, tax,bal;
        getline(iss,idprimary,'|');
        long long int idprim=stoll(idprimary);
        getline(iss,idsecondary,'|');
        long long int idsec=stoll(idsecondary);
        getline(iss,idcustomer,'|');
        long long int idcus=stoll(idcustomer);

        getline(iss,name,'|');
        //cout<<name;
        getline(iss,tax,'|');
        int taxx=stoi(tax);
        getline(iss,bal);
        long double ball=stold(bal);
        Zapis* novi=new Zapis(idcus,idprim,idsec,name,taxx,ball);
        root=dodajZapisUStablo(root,novi, red);
    }
    datoteka.close();
    return root;
}
//do ovde su definije struktura i kreiranje stabla iz datoteke
//--------------------------------------------------------------------------------------
void ispisiStablo(Node*root, int red){
    queue<Node*>q, sl;
    Node*temp=root;
    if(temp== nullptr){
        cout<<"Stablo je vec izbrisano iz memorije!"<<endl;
        return;
    }
    q.push(temp);
    while(!q.empty()){
        temp=q.front();
        q.pop();
        if(temp) {
            for (int i = 0; i < temp->n; i++) {

                if(i<temp->n-1) {
                    cout << temp->kljucevi[i]->idCustomer << "  |  ";
                }
                else
                    cout << temp->kljucevi[i]->idCustomer;
            }
            cout<<"  ";
            for(int i=0;i<red;i++){
                if(temp->podstabla[i])
                {
                    sl.push(temp->podstabla[i]);
                }
            }
            if(q.empty()){
                cout<<endl;
                q=sl;
                while(!sl.empty()){
                    sl.pop();
                }
            }
        }

    }
}
Node* obrisiStablo(Node*root, int red){
    queue<Node*>q;
    Node*temp=root;
    q.push(temp);
    while(!q.empty()){
        temp=q.front();
        q.pop();
        if(temp) {
            for(int i=0;i<red;i++){
                if(temp->podstabla[i])
                {
                    q.push(temp->podstabla[i]);
                }
            }
        }
        for(int i=0;i<temp->n;i++){
            Zapis*tmp=temp->kljucevi[i];
            while(tmp){
                Zapis* cur=tmp;
                tmp=tmp->next;
                delete cur;
            }
        }
        delete[]temp->kljucevi;
        delete []temp->podstabla;

    }
    return nullptr;
}
//---------------------------------------------------------------------------------------------------------
//nova stavka, dodavanje novog racuna u stablo sa standardnog ulaza
Node* dodajNoviSTD(Node*root, int red){
    cout<<"Unesite sve podatke o racunu koji zelite da dodate: "<<endl;
    cout<<"Unesite CA_ID:"<<endl;
    long long int caid, cabid,cacid;
    string name;
    int taxx;
    long double ball;
    cin>>caid;
    cout<<"Unesite CA_B_ID:"<<endl;
    cin>>cabid;
    cout<<"Unesite CA_C_ID:"<<endl;
    cin>>cacid;
    cin.ignore();
    cout<<"Unesite CA_NAME:"<<endl;
    getline(cin,name);
    cout<<"Unesite CA_TAX_ST:"<<endl;
    cin>>taxx;
    cout<<"Unesite CA_BAL:"<<endl;
    cin>>ball;
    Zapis*novi =new Zapis(cacid,caid,cabid,name,taxx,ball);
    root=dodajZapisUStablo(root,novi,red);
    return root;
}
//-------------------------------------------------------------
//ulancavanje Customera na pocetku
Customer* ulancajCustomera(Customer*lista, Customer*cus){
    if(lista== nullptr){
        lista=cus;
    }
    else{
        Customer*tmp=lista;
        while(tmp->next){
            tmp=tmp->next;
        }
        tmp->next=cus;
    }
    return lista;
}
Customer*procitajCustomere(string dat){
    Customer*lista= nullptr;
    ifstream datoteka;
    datoteka.open(dat);
    string line;
    while(getline(datoteka,line)){
        istringstream iss(line);
        string idprimary, fname, lname, email,caid;
        getline(iss,idprimary,'|');
        long long int idprim=stoll(idprimary);
        getline(iss,fname,'|');
        getline(iss,lname,'|');
        getline(iss,email,'|');
        getline(iss,caid,'|');
        long long int caIId=stoll(caid);
        Customer* novi=new Customer(idprim,fname, lname,email,caIId);
        lista=ulancajCustomera(lista,novi);
    }
    datoteka.close();
    return lista;
}
int pronadjiKorisnikaIIspisiNjegoveRacune(Node*root,Customer*zapis, int red, string file){
    ofstream outputFile;
    int brKorakaPretrage=0;
    outputFile.open(file);
    Node*tmp=root;
    if(root== nullptr){
        outputFile<<zapis->cId<<"|"<<zapis->cfname<<"|"<<zapis->clname<<"|"<<zapis->email<<
        "|null|null|null|null"<<endl;
        return 1;
    }
    else{
        while(tmp){
            int j=0;
            brKorakaPretrage++;
            while(j<tmp->n and tmp->kljucevi[j] and tmp->kljucevi[j]->idCustomer<zapis->cId) {
                j++;
            }

            if(tmp->kljucevi[j] and tmp->kljucevi[j]->idCustomer==zapis->cId){
                Zapis*tren=tmp->kljucevi[j];
                while(tren){
                    outputFile<<zapis->cId<<"|"<<zapis->cfname<<"|"<<zapis->clname<<"|"<<zapis->email<<"|"<<
                    tren->idPrimary<<"|"<<tren->idCustomer<<"|"<<tren->tax<<"|"<<fixed<<setprecision(2)<<tren->bal<<endl;
                    tren=tren->next;
                }
                return brKorakaPretrage;
            }
            else{
                tmp=tmp->podstabla[j];
            }
        }
        outputFile<<zapis->cId<<"|"<<zapis->cfname<<"|"<<zapis->clname<<"|"<<zapis->email<<
                  "|null|null|null|null"<<endl;
    }
    outputFile.close();
    return brKorakaPretrage;
}
void pronadjiSveRacuneZadatogKorisnika(Node*root,Customer*lista,long long int id, int red){
    Customer*tmp=lista;
    int br=0;
    string fileName = "rezultat.txt";
    while(tmp){
        if(tmp->cId==id){
            br=pronadjiKorisnikaIIspisiNjegoveRacune(root,tmp,red,fileName);
            break;
        }
        tmp=tmp->next;
    }
    if(tmp== nullptr)cout<<"Customer nije u listi customera!"<<endl;
    else{
        cout<<"Rezultat je kreiran!"<<endl;
        cout<<"Broj koraka pretrage:"<<br<<endl;
    }
}
//----------------------------------------------------------------------------------------------------------------------------
//sledeca celina-brisanje iz stabla
bool isti(Zapis* tmp, Zapis* zapis){
    if(tmp->idCustomer==zapis->idCustomer and tmp->idPrimary==zapis->idPrimary and tmp->idSecondary==zapis->idSecondary and tmp->name==zapis->name and tmp->tax==zapis->tax and tmp->bal==zapis->bal)
    {
        return true;
    }
    return false;
}
int findIndex(Node* node, Zapis*zapis){
    int ind;
    for(ind=0;ind<node->n;ind++){
        if(node->kljucevi[ind] and node->kljucevi[ind]->idCustomer==zapis->idCustomer){
            Zapis*tmp=node->kljucevi[ind];
            while(tmp){
                if(isti(tmp,zapis)){
                    return ind;
                }
                tmp=tmp->next;
            }
            return -1;
        }
    }
    return -1;
}
Node* sledbenik(Node*cvor, int index,Zapis* zapis){
    Node*tmp=cvor->podstabla[index+1];
    while(tmp->podstabla[0] and !tmp->isList){
        tmp=tmp->podstabla[0];
    }
    cvor->kljucevi[index]=tmp->kljucevi[0];
    tmp->kljucevi[0]=zapis;
    cvor=tmp;
    return cvor;
}
//do ovde je pretraga bez handlovanja underflow
//dalje je UNDERFLOW
//pronalanjenje koje podstablo po redu je trenutni cvo svog oca
int findParentIndex(Node*cvor){
    Node*otac=cvor->parent;
    if(otac) {
        int i;
        for (i = 0; i < otac->n + 1; i++) {
            if (otac->podstabla[i] and otac->podstabla[i] == cvor) {
                return i;
            }
        }
    }
    //ako nije u ocu, ne bi trebalo nikad da se ispuni ovaj deo, jedino kad je otac null
    return -1;
}
//****************************
//sada trazimo bracu
Node*nadjiDesnogBrata(Node*cvor){
    Node*otac=cvor->parent;
    int i= findParentIndex(cvor);
    if(i+1<=otac->n) {
        if (otac->podstabla[i + 1])
            return otac->podstabla[i + 1];
        else return nullptr;
    }
    else return nullptr;
}
Node*nadjiLevogBrata(Node*cvor){
    Node*otac=cvor->parent;
    int i= findParentIndex(cvor);
    if(i-1>=0) {
        if (otac->podstabla[i - 1])
            return otac->podstabla[i - 1];
        else return nullptr;
    }
    else return nullptr;
}
//***************************************
void removeKey(Node**node, int index){
    if(index==0){
        for(int i=0;i<(*node)->n-1;i++)
        {
            (*node)->kljucevi[i]=(*node)->kljucevi[i+1];
        }
        (*node)->kljucevi[(*node)->n-1]= nullptr;
        for(int i=0;i<(*node)->n;i++)
        {
            (*node)->podstabla[i]=(*node)->podstabla[i+1];
        }
        (*node)->podstabla[(*node)->n]= nullptr;
        (*node)->n--;
    }
    else if(index==(*node)->n-1){
        (*node)->kljucevi[index]= nullptr;
        (*node)->podstabla[index+1]= nullptr;
        (*node)->n--;
    }
}

Node*borrowKeyFromRightSibling(Node*cvor){
    Node*right= nadjiDesnogBrata(cvor);
    Node*otac=cvor->parent;
    int borrowIndex =0;
    int parentIndex=findParentIndex(cvor);
    cvor->n++;
    cvor->kljucevi[cvor->n-1]=cvor->parent->kljucevi[parentIndex];
    (otac)->kljucevi[parentIndex]=right->kljucevi[borrowIndex];
    cvor->podstabla[cvor->n]=right->podstabla[borrowIndex];
    removeKey(&right,0);
    return cvor;
}
Node*borrowKeyFromLeftSibling(Node*cvor){
    Node*left= nadjiLevogBrata(cvor);
    Node*otac=cvor->parent;
    int borrowIndex=left->n-1;
    int parentIndex= findParentIndex(cvor);
    cvor->n++;
    for(int j=cvor->n-1;j>0;j--){
        cvor->kljucevi[j]=cvor->kljucevi[j-1];
    }
    cvor->kljucevi[0]=cvor->parent->kljucevi[parentIndex-1];
    for(int j=cvor->n;j>0;j--){
        cvor->podstabla[j]=cvor->podstabla[j-1];
    }
    cvor->podstabla[0]=left->podstabla[borrowIndex+1];
    otac->kljucevi[parentIndex-1]=left->kljucevi[borrowIndex];
    removeKey(&left,borrowIndex);
    return cvor;

}
void promeniOcaD(Node** otac,int parentIndex){
    int i;
    if(*otac) {
        for (i = parentIndex; i < (*otac)->n - 1; i++) {
            (*otac)->kljucevi[i] = (*otac)->kljucevi[i + 1];
        }
        (*otac)->kljucevi[(*otac)->n - 1] = nullptr;
        for (i = parentIndex + 1; i < (*otac)->n; i++) {
            (*otac)->podstabla[i] = (*otac)->podstabla[i + 1];
        }
        (*otac)->podstabla[(*otac)->n] = nullptr;

        (*otac)->n--;
    }
}
Node* mergeWithRight(Node*cvor){
    Node*desni= nadjiDesnogBrata(cvor);
    int parentIndex= findParentIndex(cvor);
    Node*otac=cvor->parent;
    Zapis**niz=new Zapis*[2*cvor->minBrPodstabala-2];

    int i=0;
    while(i<cvor->n){
        niz[i]=cvor->kljucevi[i];
        i++;
    }
    niz[i++]=otac->kljucevi[parentIndex];
    int br=i;
    while(i<2*cvor->minBrPodstabala-2){
        niz[i]=desni->kljucevi[i-br];
        i++;
    }
    for(i=0;i<2*cvor->minBrPodstabala-2;i++){
        cvor->kljucevi[i]=niz[i];
    }
    int m=0;
    while(cvor->podstabla[m]){
        m++;
    }
    br=m;
    while(desni->podstabla[m-br]){
        cvor->podstabla[m]=desni->podstabla[m-br];
        desni->podstabla[m-br]->parent=cvor;
        //desni->podstabla[m-br]= nullptr;
        m++;
    }
    cvor->n=2*cvor->minBrPodstabala-2;
    promeniOcaD(&otac,parentIndex);
    return cvor;

}
void promeniOcaL(Node**otac, int parentIndex){
    int i;
    if((*otac)) {
        for (i = parentIndex - 1; i < (*otac)->n - 1; i++) {
            (*otac)->kljucevi[i] = (*otac)->kljucevi[i + 1];
        }
        (*otac)->kljucevi[(*otac)->n - 1] = nullptr;
        for (i = parentIndex - 1; i < (*otac)->n; i++) {
            (*otac)->podstabla[i] = (*otac)->podstabla[i + 1];
        }
        (*otac)->podstabla[(*otac)->n] = nullptr;
        (*otac)->n--;
    }
}
Node* mergeWithLeft(Node*cvor){
    Node*left= nadjiLevogBrata(cvor);
    Node*otac=cvor->parent;
    int parentIndex= findParentIndex(cvor);
    Zapis**niz=new Zapis*[2*cvor->minBrPodstabala-2];
    int i=0;
    while(i<left->n){
        niz[i]=left->kljucevi[i];
        i++;
    }
    niz[i++]=otac->kljucevi[parentIndex-1];
    int br=i;
    while(i<2*cvor->minBrPodstabala-2){
        niz[i]=cvor->kljucevi[i-br];
        i++;
    }
    for(i=0;i<2*cvor->minBrPodstabala-2;i++){
        cvor->kljucevi[i]=niz[i];
    }
    int m=0;
    Node*niz1[2*cvor->minBrPodstabala-1];
    while(left->podstabla[m]){
        niz1[m]=left->podstabla[m];
        m++;
    }
    br=m;
    while(cvor->podstabla[m-br]){
        niz1[m]=cvor->podstabla[m-br];
        m++;
    }
    br=m;
    m=0;
    while(m<br){
        cvor->podstabla[m]=niz1[m];
        niz1[m]->parent=cvor;
        //if(left->podstabla[m])left->podstabla[m]= nullptr;
        m++;
    }
    cvor->n=2*cvor->minBrPodstabala-2;
    promeniOcaL(&otac,parentIndex);
    return cvor;
}
//merge sa desnim ako ga ima ako ga nema sa levim, menjamo cvor, gubi se desni tj levi, dealocira se a njegove cvorove i roditeljski
//zajednicki stavljamo u cvor
Node*mergeWithSibling(Node* cvor){
    Node*desni= nadjiDesnogBrata(cvor);
    Node*levi= nadjiLevogBrata(cvor);
    if(desni) {
        cvor=mergeWithRight(cvor);
    }
    else if(levi) {
        cvor=mergeWithLeft(cvor);
    }
    return cvor;
}
//ako ima desnog brata koji moze da pozajmi pozajmljuje od njega, ako nema onda pokusa od levog, ako ne moze ni od jednog
//merge sa desnim bratom ako ga ima, ako ga nema sa levim
Node* handleUnderflow(Node* cvor){
    Node*desniBrat=nadjiDesnogBrata(cvor);
    Node*leviBrat=nadjiLevogBrata(cvor);
    if ( desniBrat and desniBrat->n>desniBrat->minBrKljucevaUListu)
    {
        cvor = borrowKeyFromRightSibling(cvor);
    }
    else if (leviBrat and leviBrat->n > leviBrat->minBrKljucevaUListu)
    {
        cvor = borrowKeyFromLeftSibling(cvor);
    }
    else
    {
        cvor = mergeWithSibling(cvor);
    }
    return  cvor;
}
//sve pre handle underflow
Zapis* zapisStd(){
    cout<<"Unesite sve podatke o racunu koji zelite da obrisete: "<<endl;
    cout<<"Unesite CA_ID:"<<endl;
    long long int caid, cabid,cacid;
    string name;
    int tax;
    long double bal;
    cin>>caid;
    cout<<"Unesite CA_B_ID:"<<endl;
    cin>>cabid;
    cout<<"Unesite CA_C_ID:"<<endl;
    cin>>cacid;
    cin.ignore();
    cout<<"Unesite CA_NAME:"<<endl;
    getline(cin,name);
    cout<<"Unesite CA_TAX_ST:"<<endl;
    cin>>tax;
    cout<<"Unesite CA_BAL:"<<endl;
    cin>>bal;
    Zapis*novi =new Zapis(cacid,caid,cabid,name,tax,bal);
    return novi;
}
bool izbrisiZapis(Node**node,int index,Zapis* zapis){
    Zapis*p=(*node)->kljucevi[index],*q= nullptr;
    while(p){
        if(isti(p,zapis)){
            if(q) {
                q->next= p->next;
                return true;
            }
            else{
                (*node)->kljucevi[index]=p->next;
                return true;
            }
        }
        q=p;
        p=p->next;
    }
    return false;
}
bool izbrisiKljuc(Node** cvor, int index, Zapis* zapis){
    if(!(*cvor)->isList)
    {
        *cvor=sledbenik(*cvor,index,zapis);
        index=0;
    }
    Zapis*tmp=(*cvor)->kljucevi[index];
    if (tmp->next == nullptr and isti(tmp, zapis)) {
        for (int j = index; j < (*cvor)->n - 1; j++) {
            (*cvor)->kljucevi[j] = (*cvor)->kljucevi[j + 1];
        }
        (*cvor)->kljucevi[(*cvor)->n - 1] = nullptr;
        (*cvor)->n--;
        return true;
    }
    else {
        return false;
    }
}
Node* obrisiKljucIzStabla(Node*root, int red){
    int br=0;
    Zapis*novi=zapisStd();
    Node*node=pretraziStabloNaZapis(root,novi,red,br);
    int index=findIndex(node,novi);
    if(index==-1) {
        cout<<"Racun se ne nalazi u stablu!"<<endl;
        cout<<"Broj koraka pretrage:"<<br<<endl;
        return root;
    }
    else{
        if(node->kljucevi[index]->next!= nullptr)
        {
            bool izbrisan=izbrisiZapis(&node,index,novi);
            if(izbrisan){
                cout<<"Zapis je izbrisan iz memorije!"<<endl;
                cout<<"Broj koraka pretrage:"<<br<<endl;
                return root;
            }
            else{
                cout<<"Racun se ne nalazi u stablu!"<<endl;
                cout<<"Broj koraka pretrage:"<<br<<endl;
                return root;
            }
        }
        else
        {
            //brisemo ceo kljuc iz cvora, nakon toga sve dok je underflow resavamo to
            bool izbrisan=izbrisiKljuc(&node, index, novi);
            if(izbrisan)
            {
                while(node!=root and node->n<node->minBrKljucevaUListu){
                    node= handleUnderflow(node);
                    node=node->parent;
                }
                if(node==root  and root->podstabla[0] and root->podstabla[1]== nullptr) {
                    root->n=root->podstabla[0]->n;
                    root = root->podstabla[0];
                    root->parent= nullptr;
                }
                cout<<"Zapis je izbrisan iz memorije!"<<endl;
                cout<<"Broj koraka pretrage:"<<br<<endl;
                return root;
            }
            else
            {
                cout<<"Racun se ne nalazi u stablu!"<<endl;
                cout<<"Broj koraka pretrage:"<<br<<endl;
                return root;
            }
        }
    }
}
//izbacivanje svih racuna jednog CUSTOMERA
//--------------------------------------------------------------------------------------------------------------
Node*pretraziStabloNaId(Node*root,long long int cId,int&br){
    if(root->n==0)return root;
    Node* temp=root;
    while(temp!= nullptr) {
        br++;
        int i=0;
        while(i<temp->n and temp->kljucevi[i]->idCustomer<cId){
            i++;
        }
        if(i==temp->n){
            if(temp->isList)return temp;
            else
            {
                temp = temp->podstabla[i];
            }
        }
        else if(temp->kljucevi[i]->idCustomer==cId){
            return temp;
        }
        else{
            if(temp->isList)return temp;
            else
            {
                temp=temp->podstabla[i];
            }
        }
    }
    return nullptr;
}
int findIndexId(Node*node, long long int cId){
    int ind;
    for(ind=0;ind<node->n;ind++){
        if(node->kljucevi[ind] and node->kljucevi[ind]->idCustomer==cId){
           return ind;
        }
    }
    return -1;
}
bool izbrisiKljucPoId(Node**cvor,int index, long long int cId){
    Zapis*tmp=(*cvor)->kljucevi[index];
    if(tmp->idCustomer==cId) {
        cout<<tmp->idPrimary<<"|"<<tmp->idSecondary<<"|"<<tmp->idCustomer<<"|"<<tmp->name<<"|"<<tmp->tax<<"|"<<fixed<<setprecision(2)<<tmp->bal<<endl;
        if (tmp->next != nullptr) {
            Zapis *novi = tmp->next;
            while (novi) {
                Zapis *p = novi;
                novi = novi->next;
                cout<<p->idPrimary<<"|"<<p->idSecondary<<"|"<<p->idCustomer<<"|"<<p->name<<"|"<<p->tax<<"|"<<fixed<<setprecision(2)<<p->bal<<endl;
                delete (p);
            }
            tmp->next= nullptr;
        }
        if(!(*cvor)->isList){
            *cvor=sledbenik(*cvor,index,tmp);
            index=0;
        }
        Zapis*tmp1=(*cvor)->kljucevi[index];
        if (tmp1->next == nullptr and tmp1->idCustomer==cId) {
            for (int j = index; j < (*cvor)->n - 1; j++) {
                (*cvor)->kljucevi[j] = (*cvor)->kljucevi[j + 1];
            }
            (*cvor)->kljucevi[(*cvor)->n - 1] = nullptr;
            (*cvor)->n--;
            return true;
        }
        else {
            return false;
        }
    }
    return false;
}
Node* obrisiSveRacuneJednogKorisnika(Node*root,long long int cId){
    int br=0;
    Node*node=pretraziStabloNaId(root,cId,br);
    int index=findIndexId(node,cId);
    if(index==-1) {
        cout<<"Racun se ne nalazi u stablu!"<<endl;
        cout<<"Broj koraka pretrage:"<<br<<endl;
        return root;
    }
    else{
        //brisemo ceo kljuc iz cvora, nakon toga sve dok je underflow resavamo to
        bool izbrisan=izbrisiKljucPoId(&node, index, cId);
        if(izbrisan)
        {
            while(node!=root and node->n<node->minBrKljucevaUListu){
                node= handleUnderflow(node);
                node=node->parent;
            }
            if(node==root  and root->podstabla[0] and root->podstabla[1]== nullptr) {
                root->n=root->podstabla[0]->n;
                root = root->podstabla[0];
                root->parent= nullptr;
            }
            cout<<"Zapisi su izbrisani iz stabla!"<<endl;
            cout<<"Broj koraka pretrage:"<<br<<endl;
            return root;
        }
        else
        {
            cout<<"Racun se ne nalazi u stablu!"<<endl;
            cout<<"Broj koraka pretrage:"<<br<<endl;
            return root;
        }
    }
}
//--------------------------------------------------------------------------------------------------------------
//main
int main() {
    cout << "Dobro dosli, pritisnite 1 za pocetak rada sa bazom podataka: "<<endl;
    int a;
    cin >> a;
    Node* root;
    Customer*lista=procitajCustomere("Customer.txt");
    if (a == 1){
    while (true) {
            cout << "Izaberite jednu od sledecih opcija:\n"
                    "1.Stvaranje indeksa nad tabelom CUSTOMER_ACCOUNT\n"
                    "2.Unistavanje indeksa nad tabelom CUSTOMER_ACCOUNT\n"
                    "3.Ispis indeksa za male velicine\n"
                    "4.Ispis podataka o korisniku i pretraga svih njegovih racuna\n"
                    "5.Dodavanje novog zapisa(racuna) u indeks\n"
                    "6.Brisanje zapisa(racuna) iz indeksa\n"
                    "7.Brisanje svih racuna jednog CUSTOMERA\n"
                    "8.Prekid rada programa\n";

            int b;
            cin >> b;
            switch (b) {
                case 1:
                    cout<<"Unesite red B stabla(3-10): "<<endl;
                    int redB;
                    cin>>redB;
                    if(redB<3 or redB>10){
                        cout<<"Greska, pokusajte ponovo!"<<endl;
                        return 0;
                    }
                    root=new Node(0,redB,true);
                    root=unesiCvoroveIzDatoteke(root,redB,"CustomerAccount20.txt");
                    cout<<"Indeksna struktura nad CUSTOMER_ACCOUNT je kreirana!"<<endl;
                    break;
                case 2:
                    if(root) {
                        root=obrisiStablo(root, redB);
                        cout << "Indeksna struktura nad CUSTOMER_ACCOUNT je obrisana iz memorije!" << endl;

                    }
                    else{
                        cout<<"Stablo je vec izbrisano iz memorije!"<<endl;
                        return 0;
                    }
                    break;
                case 3:
                    ispisiStablo(root,redB);
                    break;
                case 4:
                    cout<<"Unseite primarni kljuc korisnika C_ID: "<<endl;
                    long long int id;
                    cin>>id;
                    pronadjiSveRacuneZadatogKorisnika(root, lista,id,redB);
                    break;
                case 5:
                    if(root)root=dodajNoviSTD(root,redB);
                    else{
                        cout<<"Stablo je obrisano iz memorije!"<<endl;
                    }
                    break;
                case 6:
                    if(root){
                        root=obrisiKljucIzStabla(root, redB);
                    }
                    else{
                        cout<<"Stablo je obrisano iz memorije!"<<endl;
                    }
                    break;
                case 7:
                    if(root) {
                        cout << "Unesite CA_C_ID korisnika cije sve racune zelite da obrisete: " << endl;
                        long long int cId;
                        cin >> cId;
                        root = obrisiSveRacuneJednogKorisnika(root, cId);
                    }
                    else{
                        cout<<"Stablo je obrisano iz memorije!"<<endl;
                    }
                    break;
                case 8:
                    cout << "Kraj programa" << endl;
                    if(root)root=obrisiStablo(root,redB);
                    return 0;
                default:
                    cout << "Nevalidna opcija, pokusajte ponovo!" << endl;
                    break;
            }
        }
    }
}
