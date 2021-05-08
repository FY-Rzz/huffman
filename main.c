#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <stdbool.h>

typedef struct point//字符节点
{
    char zifu;
    int degree;
    struct point *next;
}SP;
SP Head={'\0',0,NULL};
SP *p_head=&Head,*p_tail=&Head;
SP persort[128];//储存数组
int hufsize=0;

typedef struct hfstruct//树节点结构体
{
    struct hfstruct *Llink,*Rlink;//链接
    char info;//字符信息
    char bianma[15];//01编码
    int weight;//权值
}HF;
HF felem[256];//树节点存储
HF *rooot;//树根

struct hufbianma
{
    char info;
    char btis[15];
}HFB[128];
int bmcont=0;

struct filehead//文件头部信息树
{
    char flag[3];
    int filenum;//文件字符总数
    int charkindnum;//文件字符种类数
}FH;

struct filepoint
{
    char zifu;
    int degree;
}FP[128];

char hufwb[15];
unsigned char saveinfo=0,loadinfo=0;


int bianmajilu(char bm);//记录字符数
void outputzifu();//打印字符数
void hufbuild();//构树
int compare(SP x,SP y);
void hufprecoding(HF *t,char lor,int bit,char bits[15]);//分配字符编码
void hufcode(FILE *hufread);//写入压缩文本
void hufdecode(FILE *hufread);//将压缩文本解码

int main()
{
    SetConsoleTitleA("哈夫曼文件压缩程序");
    //system("mode con cols=70 lines=30 ");//设置窗口大小
    system("color 8E");//设置窗口颜色

    int ans;
    do{
        printf("============请输入您想执行的操作代号==============\n");
        printf("          0->压缩文件         1->解压文件         \n");
        scanf("%d",&ans);
        if(ans!=1&&ans!=0)
        {
            printf("请输入正确的指令！\n请重新输入：");
        }
    }while(ans!=1&&ans!=0);

    FILE *huf;
    if(!ans)
    {
        char name[1000];
        printf("请输入待压缩文件路径：\n");
        scanf("%s",name);
        if((huf=fopen(name,"rb+"))==NULL)
        {
            printf("Error!未找到相应文件。。。\n");
            printf("按任意键退出程序\n");
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
        printf("压缩成功！生成压缩文件：\"comdenced.xzw\"\n");
    }
    else
    {
        char name[1000];
        printf("请输入待解压文件路径：\n");
        scanf("%s",name);
        if((huf=fopen(name,"rb+"))==NULL)
        {
            printf("Error!未找到相应文件。。。\n");
            printf("按任意键退出程序\n");
            system("pause");
            exit(-1);
        }
        hufdecode(huf);
        fclose(huf);
        printf("解码成功！生成文件：\"decode.txt\"\n");
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
            printf(" '换行' 的个数为%d\n",p->degree);
            persort[i++]=*p;
            p=p->next;
            continue ;
        }
        if(p->zifu=='\r')
        {
            printf(" '回车' 的个数为%d\n",p->degree);
            persort[i++]=*p;
            p=p->next;
            continue ;
        }
        if(p->zifu==' ')
        {
            printf(" '空格' 的个数为%d\n",p->degree);
            persort[i++]=*p;
            p=p->next;
            continue ;
        }
        if(p->zifu=='\t')
        {
            printf(" '制表' 的个数为%d\n",p->degree);
            persort[i++]=*p;
            p=p->next;
            continue ;
        }
        printf(" '%c'    的个数为%d\n",p->zifu,p->degree);
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

void hufbuild()//构树
{
    hufsize=FH.charkindnum;
    qsort(persort,hufsize,sizeof(SP),compare);//节点排序
    HF *elem[128];
    for(int i=0;i<hufsize;i++)//初始化
    {
        felem[i].info=persort[i].zifu;
        felem[i].weight=persort[i].degree;
        felem[i].Llink=felem[i].Rlink=NULL;
        elem[i]=&felem[i];
    }
    for(int i=0;i<hufsize-1;i++)//组合
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

void hufprecoding(HF *t,char lor,int bit,char bits[15])//先根遍历分配字符编码
{
    if(t==NULL)
        return ;//递归出口
    if(bit>=0)
    {strcpy(t->bianma,bits);
    t->bianma[bit]=lor;//访问并编码
    t->bianma[bit+1]='\0';}

    hufprecoding(t->Llink,'0',bit+1,t->bianma);
    hufprecoding(t->Rlink,'1',bit+1,t->bianma);
    if(t->Llink==NULL)
    {
        HFB[bmcont].info=t->info;
        strcpy(HFB[bmcont++].btis,t->bianma);
    }
}

void hufcode(FILE *hufread)//写入压缩文本
{
    int beforeword=0,afterword=0;
    float percent;
    FILE *hufwrite=fopen("comdenced.xzw","wb");
    char tmp;
    int k=0,num=0;//计数用

    FH.flag[0]='X';FH.flag[1]='Z';FH.flag[2]='W';//头节点标记
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
         for(int i=0;i<hufsize;i++)//将对应字符串编码录入hufwb
         {
             if(tmp==HFB[i].info)
             {
                 strcpy(hufwb,HFB[i].btis);
                 break ;
             }
         }
         //将字符串编码转换成位编码


        k=strlen(hufwb);
         for(int i=0;i<k;i++)
        {
            saveinfo=((hufwb[i]-'0')|saveinfo);//让saveinfo和编码中的每一位进行或操作
            num++;
            if(num==8)
            {
                //printf("%d ",saveinfo);
                fwrite(&saveinfo,sizeof(unsigned char),1,hufwrite);//每8位写入一次文件
                saveinfo=0;//重新置0
                num=0;
                afterword++;
            }
            else
            {
                saveinfo=saveinfo << 1; //每做完一步，左移一位
            }
        }
    }
    fclose(hufwrite);
    fclose(hufread);
    percent=(float)afterword/(float)beforeword;
    printf("压缩完成，压缩比为 %.2f %%\n",percent*100);
}

void hufdecode(FILE *hufread)//将压缩文本解码
{
    HF *recpoint=rooot;
    FILE *hufwrite=fopen("decoded.txt","wb+");
    fread(&FH,sizeof(struct filehead),1,hufread);//读取头部信息
    if(FH.flag[0]=='X'&&FH.flag[1]=='Z'&&FH.flag[2]=='W')
        ;
    else
    {
        printf("文件格式无法识别！请输入正确的文件信息\n");
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
    hufbuild();//重新构树

    recpoint=rooot;
    int i=0,num=0;
    while(1)//读取一个8位字符
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


