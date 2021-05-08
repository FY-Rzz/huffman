#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <stdbool.h>

typedef struct point//�ַ��ڵ�
{
    char zifu;
    int degree;
    struct point *next;
}SP;
SP Head={'\0',0,NULL};
SP *p_head=&Head,*p_tail=&Head;
SP persort[128];//��������
int hufsize=0;

typedef struct hfstruct//���ڵ�ṹ��
{
    struct hfstruct *Llink,*Rlink;//����
    char info;//�ַ���Ϣ
    char bianma[15];//01����
    int weight;//Ȩֵ
}HF;
HF felem[256];//���ڵ�洢
HF *rooot;//����

struct hufbianma
{
    char info;
    char btis[15];
}HFB[128];
int bmcont=0;

struct filehead//�ļ�ͷ����Ϣ��
{
    char flag[3];
    int filenum;//�ļ��ַ�����
    int charkindnum;//�ļ��ַ�������
}FH;

struct filepoint
{
    char zifu;
    int degree;
}FP[128];

char hufwb[15];
unsigned char saveinfo=0,loadinfo=0;


int bianmajilu(char bm);//��¼�ַ���
void outputzifu();//��ӡ�ַ���
void hufbuild();//����
int compare(SP x,SP y);
void hufprecoding(HF *t,char lor,int bit,char bits[15]);//�����ַ�����
void hufcode(FILE *hufread);//д��ѹ���ı�
void hufdecode(FILE *hufread);//��ѹ���ı�����

int main()
{
    SetConsoleTitleA("�������ļ�ѹ������");
    //system("mode con cols=70 lines=30 ");//���ô��ڴ�С
    system("color 8E");//���ô�����ɫ

    int ans;
    do{
        printf("============����������ִ�еĲ�������==============\n");
        printf("          0->ѹ���ļ�         1->��ѹ�ļ�         \n");
        scanf("%d",&ans);
        if(ans!=1&&ans!=0)
        {
            printf("��������ȷ��ָ�\n���������룺");
        }
    }while(ans!=1&&ans!=0);

    FILE *huf;
    if(!ans)
    {
        char name[1000];
        printf("�������ѹ���ļ�·����\n");
        scanf("%s",name);
        if((huf=fopen(name,"rb+"))==NULL)
        {
            printf("Error!δ�ҵ���Ӧ�ļ�������\n");
            printf("��������˳�����\n");
            system("pause");
            exit(-1);
        }
        char tmp;
        while((tmp=fgetc(huf))!=EOF)
        {
            FH.filenum++;
            bianmajilu(tmp);
        }
        fclose(huf);

        outputzifu();
        hufbuild();
        hufprecoding(rooot,'0',-1,"\0");
        huf=fopen(name,"rb+");
        hufcode(huf);
        fclose(huf);
        printf("ѹ���ɹ�������ѹ���ļ���\"comdenced.xzw\"\n");
    }
    else
    {
        char name[1000];
        printf("���������ѹ�ļ�·����\n");
        scanf("%s",name);
        if((huf=fopen(name,"rb+"))==NULL)
        {
            printf("Error!δ�ҵ���Ӧ�ļ�������\n");
            printf("��������˳�����\n");
            system("pause");
            exit(-1);
        }
        hufdecode(huf);
        fclose(huf);
        printf("����ɹ��������ļ���\"decode.txt\"\n");
    }
    return 0;
}

int bianmajilu(char bm)
{
    SP *p=p_head;
    while(p)
    {
        if(p->zifu==bm)
        {
            p->degree++;
            return 1;
        }
        p=p->next;
    }
    SP *tmp=(SP*)malloc(sizeof(SP));
    tmp->zifu=bm;
    tmp->degree=1;
    tmp->next=NULL;
    p_tail->next=tmp;
    p_tail=tmp;
    return 2;
}

void outputzifu()
{
    SP *p=p_head->next;
    int i=0;
    while(p!=NULL)
    {
        printf("%d ",i);
        if(p->zifu=='\n')
        {
            printf(" '����' �ĸ���Ϊ%d\n",p->degree);
            persort[i++]=*p;
            p=p->next;
            continue ;
        }
        if(p->zifu=='\r')
        {
            printf(" '�س�' �ĸ���Ϊ%d\n",p->degree);
            persort[i++]=*p;
            p=p->next;
            continue ;
        }
        if(p->zifu==' ')
        {
            printf(" '�ո�' �ĸ���Ϊ%d\n",p->degree);
            persort[i++]=*p;
            p=p->next;
            continue ;
        }
        if(p->zifu=='\t')
        {
            printf(" '�Ʊ�' �ĸ���Ϊ%d\n",p->degree);
            persort[i++]=*p;
            p=p->next;
            continue ;
        }
        printf(" '%c'    �ĸ���Ϊ%d\n",p->zifu,p->degree);
        persort[i++]=*p;
        p=p->next;
    }
    hufsize=i;
    FH.charkindnum=i;
    return ;
}

int compare(SP x,SP y)
{
    if(x.degree<y.degree)
        return -1;
    else
        return 1;
}

void hufbuild()//����
{
    hufsize=FH.charkindnum;
    qsort(persort,hufsize,sizeof(SP),compare);//�ڵ�����
    HF *elem[128];
    for(int i=0;i<hufsize;i++)//��ʼ��
    {
        felem[i].info=persort[i].zifu;
        felem[i].weight=persort[i].degree;
        felem[i].Llink=felem[i].Rlink=NULL;
        elem[i]=&felem[i];
    }
    for(int i=0;i<hufsize-1;i++)//���
    {
        HF hufroot;
        hufroot.Llink=elem[0];
        hufroot.Rlink=elem[1];
        hufroot.weight=elem[0]->weight+elem[1]->weight;
        felem[hufsize+i]=hufroot;
        int tag=1;
        for(int k=2;k<hufsize-i;k++)
        {

            if(elem[k]->weight<hufroot.weight)
            {
                elem[k-2]=elem[k];
            }
            else
            {
                if(tag)
                {
                    elem[k-2]=&felem[hufsize+i];
                    tag=0;
                }
                elem[k-1]=elem[k];
            }
        }
        if(tag)
        {
            elem[hufsize-i-2]=&felem[hufsize+i];
        }
    }
    rooot=elem[0];
}

void hufprecoding(HF *t,char lor,int bit,char bits[15])//�ȸ����������ַ�����
{
    if(t==NULL)
        return ;//�ݹ����
    if(bit>=0)
    {strcpy(t->bianma,bits);
    t->bianma[bit]=lor;//���ʲ�����
    t->bianma[bit+1]='\0';}

    hufprecoding(t->Llink,'0',bit+1,t->bianma);
    hufprecoding(t->Rlink,'1',bit+1,t->bianma);
    if(t->Llink==NULL)
    {
        HFB[bmcont].info=t->info;
        strcpy(HFB[bmcont++].btis,t->bianma);
    }
}

void hufcode(FILE *hufread)//д��ѹ���ı�
{
    int beforeword=0,afterword=0;
    float percent;
    FILE *hufwrite=fopen("comdenced.xzw","wb");
    char tmp;
    int k=0,num=0;//������

    FH.flag[0]='X';FH.flag[1]='Z';FH.flag[2]='W';//ͷ�ڵ���
    fwrite(&FH,sizeof(struct filehead),1,hufwrite);
    for(int i=0;i<FH.charkindnum;i++)
    {
        FP[i].zifu=persort[i].zifu;
        FP[i].degree=persort[i].degree;
        fwrite(&FP[i],sizeof(struct filepoint),1,hufwrite);
    }

    saveinfo=0;
    while((tmp=fgetc(hufread))!=EOF)
    {
        beforeword++;
        //printf("%c %d\n",tmp,k++);
         for(int i=0;i<hufsize;i++)//����Ӧ�ַ�������¼��hufwb
         {
             if(tmp==HFB[i].info)
             {
                 strcpy(hufwb,HFB[i].btis);
                 break ;
             }
         }
         //���ַ�������ת����λ����


        k=strlen(hufwb);
         for(int i=0;i<k;i++)
        {
            saveinfo=((hufwb[i]-'0')|saveinfo);//��saveinfo�ͱ����е�ÿһλ���л����
            num++;
            if(num==8)
            {
                //printf("%d ",saveinfo);
                fwrite(&saveinfo,sizeof(unsigned char),1,hufwrite);//ÿ8λд��һ���ļ�
                saveinfo=0;//������0
                num=0;
                afterword++;
            }
            else
            {
                saveinfo=saveinfo << 1; //ÿ����һ��������һλ
            }
        }
    }
    fclose(hufwrite);
    fclose(hufread);
    percent=(float)afterword/(float)beforeword;
    printf("ѹ����ɣ�ѹ����Ϊ %.2f %%\n",percent*100);
}

void hufdecode(FILE *hufread)//��ѹ���ı�����
{
    HF *recpoint=rooot;
    FILE *hufwrite=fopen("decoded.txt","wb+");
    fread(&FH,sizeof(struct filehead),1,hufread);//��ȡͷ����Ϣ
    if(FH.flag[0]=='X'&&FH.flag[1]=='Z'&&FH.flag[2]=='W')
        ;
    else
    {
        printf("�ļ���ʽ�޷�ʶ����������ȷ���ļ���Ϣ\n");
        system("pause");
        exit(0);
    }
    for(int k=0;k<FH.charkindnum;k++)
    {
        fread(&FP[k],sizeof(struct filepoint),1,hufread);
    }
    for(int k=0;k<FH.charkindnum;k++)
    {
        persort[k].zifu=FP[k].zifu;
        persort[k].degree=FP[k].degree;
        persort[k].next=NULL;
    }
    hufbuild();//���¹���

    recpoint=rooot;
    int i=0,num=0;
    while(1)//��ȡһ��8λ�ַ�
    {
        fread(&loadinfo,sizeof(unsigned char),1,hufread);
        //printf("%d ",loadinfo);
        for(i=0;i<8;i++)
        {
            if((loadinfo&(1<<(0^7)))==0)
            {
                recpoint=recpoint->Llink;
            }
            else
            {
                recpoint=recpoint->Rlink;
            }
            loadinfo=loadinfo<<1;
            if(recpoint->Llink==NULL)
            {
                fprintf(hufwrite,"%c",recpoint->info);
                recpoint=rooot;
                num++;
                //if(num==FH.filenum)
                  //  break ;
            }
        }
        if(feof(hufread))//||num==FH.filenum)
            break;
    }
    fclose(hufwrite);
}


