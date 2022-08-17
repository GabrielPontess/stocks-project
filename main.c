#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <gtk/gtk.h>

GtkBuilder* builder;
GtkWidget *window,*stock_view,*box_view_menu,*button[5];
GtkWidget *popup_edit,*popup_delete;
GtkStack* stack;

GtkLabel* estoq_nome;
GtkLabel* estoq_desc;
GtkLabel* estoq_tipo;
GtkLabel* estoq_limite;

GtkMessageDialog* message_dialog;
GtkListStore* store_model;

FILE *fileIn,*fileOut;
static char this_filename[100]; 
static int  this_id;
static int ctrl_names;
static int stock_size;


int i = 0;



typedef struct Ctrl_StNames{
    char nome[25];
    char tipo[20];
    char quant[3];
    char desc[30];
}Ctrl_StNames;

Ctrl_StNames *ctrl_stNames;
Ctrl_StNames stName;

typedef struct Stock{
    int  id;
    char nome[25];
    char tipo[20];
    char desc[30];
    char quant[4];
    char valor[12];
}Stock;

Stock *this_Stock;
Stock temp_stock;


int get_ctrl_names();
void read_ctrl_names();

bool update_ctrl_names(Ctrl_StNames data);
bool create_stock_file(const char *estoque_nome);
bool add_item_in_file(Stock data);
bool verificar_id(int id);



void on_main_window_destroy(GtkWidget *widget, gpointer data);
void on_login_btn_clicked(GtkWidget *widget, gpointer data);

void login(const char *usuario,const char *senha,bool lembrar);
void mensagem(char text[100],char secondary_text[100],char image[100],char icon_name[100]);

void on_menu_btn_criar_clicked(GtkWidget *widget, gpointer data);
void on_menu_btn_atualizar_clicked(GtkWidget *widget, gpointer data);
void on_menu_btn_sair_clicked(GtkWidget *widget, gpointer data);

void on_cad_estoq_btn_confirmar_clicked(GtkWidget *widget, gpointer data);
void on_cad_estoq_btn_voltar_clicked(GtkWidget *widget, gpointer data);

void on_stock_enter(GtkWidget *widget,gpointer* St);

void on_estoq_btn_adicionar_clicked(GtkWidget *widget, gpointer data);
void on_estoq_btn_atualizar_clicked(GtkWidget *widget, gpointer data);
void on_estoq_btn_editar_clicked(GtkWidget *widget, gpointer data);
void on_estoq_btn_excluir_clicked(GtkWidget *widget, gpointer data);
void on_estoq_btn_voltar_clicked(GtkWidget *widget, gpointer data);

void on_cad_item_btn_confirm_clicked(GtkWidget *widget, gpointer data);
void on_cad_item_btn_voltar_clicked(GtkWidget *widget, gpointer data);

void on_edit_item_btn_confirm_clicked(GtkWidget *widget, gpointer data);
void on_edit_item_btn_voltar_clicked(GtkWidget *widget, gpointer data);

void on_popup_edit_btn_confirm_clicked(GtkWidget *widget, gpointer data);
void on_popup_edit_btn_voltar_clicked(GtkWidget *widget, gpointer data);

void on_popup_del_btn_confirm_clicked(GtkWidget *widget, gpointer data);
void on_popup_del_btn_voltar_clicked(GtkWidget *widget, gpointer data);


void criar_estoque(const char *estoq_nome,
                   const char *estoq_tipo,
                   const char *estoq_quantidade,
                   const char *estoq_desc);

void criar_item(const char  *item_nome,
                const char  *item_tipo,
                const char  *item_desc,
                const char   *item_quantidade,
                const char *item_valor);

int main(int argc, char *argv[]) {

    gtk_init(&argc , &argv);

    builder = gtk_builder_new_from_file("ui.glade");
    
    gtk_builder_add_callback_symbols(
        builder,
        "on_main_window_destroy",                       G_CALLBACK(on_main_window_destroy),
        "on_login_btn_clicked",                         G_CALLBACK(on_login_btn_clicked),
        "on_menu_btn_criar_clicked",                    G_CALLBACK(on_menu_btn_criar_clicked),
        "on_menu_btn_atualizar_clicked",                G_CALLBACK(on_menu_btn_atualizar_clicked),
        "on_menu_btn_sair_clicked",                     G_CALLBACK(on_menu_btn_sair_clicked),
        "on_cad_estoq_btn_confirmar_clicked",           G_CALLBACK(on_cad_estoq_btn_confirmar_clicked),
        "on_cad_estoq_btn_voltar_clicked",              G_CALLBACK(on_cad_estoq_btn_voltar_clicked),
        "on_edit_item_btn_voltar_clicked",              G_CALLBACK(on_edit_item_btn_voltar_clicked),
        "on_edit_item_btn_confirm_clicked",             G_CALLBACK(on_edit_item_btn_confirm_clicked),
        "on_estoq_btn_adicionar_clicked",               G_CALLBACK(on_estoq_btn_adicionar_clicked),
        "on_estoq_btn_atualizar_clicked",               G_CALLBACK(on_estoq_btn_atualizar_clicked),
        "on_cad_item_btn_voltar_clicked",               G_CALLBACK(on_cad_item_btn_voltar_clicked),
        "on_estoq_btn_voltar_clicked",                  G_CALLBACK(on_estoq_btn_voltar_clicked),
        "on_estoq_btn_excluir_clicked",                 G_CALLBACK(on_estoq_btn_excluir_clicked),
        "on_popup_edit_btn_confirm_clicked",            G_CALLBACK(on_popup_edit_btn_confirm_clicked),
        "on_popup_del_btn_confirm_clicked",             G_CALLBACK(on_popup_del_btn_confirm_clicked),
        "on_estoq_btn_editar_clicked",                  G_CALLBACK(on_estoq_btn_editar_clicked),
        "on_popup_edit_btn_voltar_clicked",             G_CALLBACK(on_popup_edit_btn_voltar_clicked),
        "on_popup_del_btn_voltar_clicked",              G_CALLBACK(on_popup_del_btn_voltar_clicked),
        "on_cad_item_btn_confirm_clicked",              G_CALLBACK(on_cad_item_btn_confirm_clicked),
        NULL);

    gtk_builder_connect_signals(builder,NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder,"main_window"));
    stack = GTK_STACK(gtk_builder_get_object(builder,"stack"));
    store_model = GTK_LIST_STORE(gtk_builder_get_object(builder,"stocks_list_store"));

    
    gtk_widget_show(window);
    gtk_main();


    return (0);
}

//gcc main.c -o main `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0` 

void on_main_window_destroy(GtkWidget *widget, gpointer data)
{
    printf("\n\n\n\nObrigado por utilizar o Stocks!\n\nDesenvolvido por: Gabriel Pontes\n\n");
    gtk_main_quit();
}

void on_login_btn_clicked(GtkWidget *widget, gpointer data){
    
    const char *usuario = gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "usuario_login")));
    const char *senha = gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "senha_login")));
    bool lembrar = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "lembrar_login")));

    login(usuario,senha,lembrar);
}

void on_menu_btn_criar_clicked(GtkWidget *widget, gpointer data){
    gtk_stack_set_visible_child_name(stack, "view_cadastro");
}

void login(const char *usuario,const char *senha,bool lembrar){
   
    if( (strcmp(usuario,"admin") == 0) && (strcmp(senha,"admin@2022") == 0)){    
        g_print("\n\nUsuário logado com sucesso!");
        mensagem("Bem-vindo","Usuário logado com sucesso!","pac_sucess","emblem-default");
        gtk_stack_set_visible_child_name(stack, "view_menu");        
    }

    else{
        g_print("\n\nLogin ou senha inválidos!");

        mensagem("Falha ao realizar o login","Usuário ou senha incorretos!","pac_404","dialog-warning");
    }
}


void on_cad_estoq_btn_confirmar_clicked(GtkWidget *widget, gpointer data){
    const char *estoq_nome =  gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(   builder, "cad_estoque_nome")));
    const char *estoq_tipo =  gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(   builder, "cad_estoque_tipo")));
    const char *estoq_quant = gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(   builder, "cad_estoque_quantidade_itens")));
    const char *estoq_desc =  gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(   builder, "cad_estoque_descricao")));

    criar_estoque(estoq_nome,estoq_tipo,estoq_quant,estoq_desc);
}

void criar_estoque(const char *estoq_nome,const char *estoq_tipo,const char *estoq_quant,const char *estoq_desc){
    Ctrl_StNames StNames;
    get_ctrl_names();

    strcpy(StNames.nome   ,estoq_nome);
    strcpy(StNames.tipo   ,estoq_tipo);
    strcpy(StNames.quant  ,estoq_quant);
    strcpy(StNames.desc   ,estoq_desc);

    if(create_stock_file(estoq_nome) == true){
        if(update_ctrl_names(StNames)){ 
            g_print("\nEstoque atualizado com sucesso!"); 
            g_print("\nQuantidade de Stocks cadastrados: %d",get_ctrl_names());

    
            mensagem("Estoque criado com sucesso!","Agora você pode adicionar itens ao seu estoque!","pac_sucess","emblem-default");
        }
    }

    else{
        mensagem("Falha ao criar o estoque","Já existe um estoque com esse nome!","pac_404","dialog-warning");
    }
    
    gtk_stack_set_visible_child_name(stack, "view_menu");
}

void on_cad_estoq_btn_voltar_clicked(GtkWidget *widget, gpointer data){
    gtk_stack_set_visible_child_name(stack, "view_menu");        
}

void on_menu_btn_atualizar_clicked(GtkWidget *widget, gpointer data){
    GtkWidget* box;

    box_view_menu = GTK_WIDGET(gtk_builder_get_object(builder,"view_menu"));
    GtkWidget* box_view_menu_out = GTK_WIDGET(gtk_builder_get_object(builder,"box_view_menu"));

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    
    gtk_container_add(GTK_CONTAINER(box_view_menu),box);
    gtk_widget_set_margin_top(box,10);
    gtk_widget_set_margin_start(box,100);
    gtk_widget_set_margin_end(box,100);
    
    get_ctrl_names();
    read_ctrl_names();

    for( i  ; i < ctrl_names ; i++){
        button[i] = gtk_button_new_with_label(ctrl_stNames[i].nome);

        gtk_widget_set_size_request(button[i],20,80);
        g_signal_connect(button[i],"clicked",G_CALLBACK(on_stock_enter),ctrl_stNames[i].nome);

        gtk_box_pack_start(GTK_BOX(box),button[i],FALSE,TRUE,5);
    }
    


    gtk_widget_show_all(box_view_menu);
    gtk_widget_hide(box_view_menu_out);
}


void mensagem(char text[100],char secondary_text[100],char image[100],char icon_name[100]){
    message_dialog = GTK_MESSAGE_DIALOG(gtk_builder_get_object(builder,"mensagem"));
    GtkImage* img = GTK_IMAGE(gtk_builder_get_object(builder,image));
    
    g_object_set(message_dialog,"icon_name",icon_name,NULL);
    g_object_set(message_dialog,"image",img,NULL);
    g_object_set(message_dialog,"text",text,NULL);
    g_object_set(message_dialog,"secondary_text",secondary_text,NULL);


    gtk_widget_show_all     (GTK_WIDGET(message_dialog));
    gtk_dialog_run          (GTK_DIALOG(message_dialog));
    gtk_widget_hide         (GTK_WIDGET(message_dialog));
}


void on_menu_btn_sair_clicked(GtkWidget *widget, gpointer data){
    printf("\n\n\n\nObrigado por utilizar o Stocks!\n\nDesenvolvido por: Gabriel Pontes\n\n");
    gtk_main_quit();
}

int get_ctrl_names(){
    size_t ctrl_names_size;

    fileIn = fopen("data/storage/ctrl_names.bin", "rb");

    if(fileIn == NULL) return 0;

    fseek(fileIn,0L,SEEK_END);
    ctrl_names_size = ftell(fileIn);
    ctrl_names = (ctrl_names_size/sizeof(Ctrl_StNames));
    fseek(fileIn,0L,SEEK_SET);

    fclose(fileIn);

    return ctrl_names;
}

void read_ctrl_names(){
    int j = 0;
    get_ctrl_names();
    int ctrl_names_size = ctrl_names;

    fileIn = fopen("data/storage/ctrl_names.bin", "rb");

    ctrl_stNames = (Ctrl_StNames *) malloc(sizeof(Ctrl_StNames)*ctrl_names);

    if(&ctrl_names_size == NULL) g_print("Falha ao ler os estoques guardados");    
    
    else
    {
        while(fread(&stName,sizeof(Ctrl_StNames),1,fileIn)){
            ctrl_stNames[j] = stName;
            j++;
        }
    }
    fclose(fileIn);
}

int get_stock_size(){
    char dir[150];
    size_t temp_stock_size;
    strcat(strcat(strcpy(dir,"data/storage/"),this_filename),".bin");
    fileIn = fopen(dir, "rb");

    if(fileIn == NULL) return 0;

    fseek(fileIn,0L,SEEK_END);
    temp_stock_size = ftell(fileIn);
    temp_stock_size = (temp_stock_size/sizeof(Stock));
    stock_size = temp_stock_size;
    fseek(fileIn,0L,SEEK_SET);

    fclose(fileIn);

    return stock_size;
}

void read_stock_itens(){
    int j = 0;
    char dir[150];
    get_stock_size();
    int stock_size = stock_size;
    
    strcat(strcat(strcpy(dir,"data/storage/"),this_filename),".bin");
    fileIn = fopen(dir,"rb");

    this_Stock = (Stock *) malloc(sizeof(Stock)*stock_size);

    while(fread(&temp_stock,sizeof(Stock),1,fileIn)){
    this_Stock[j] = temp_stock;
    j++;
    }

    fclose(fileIn);
}

bool update_ctrl_names(Ctrl_StNames StName){
    fileOut = fopen("data/storage/ctrl_names.bin", "ab");

    if(fileOut == NULL) return false;

    if(fwrite(&StName, sizeof(Ctrl_StNames),1,fileOut) != 1) return false;

    if(fclose(fileOut) == EOF) return false;

    return true;
}

bool create_stock_file(const char *estoque_nome){
    char dir[100];

    strcat(strcat(strcpy(dir,"data/storage/"),estoque_nome),".bin");
    fileOut = fopen(dir,"wb");

    if(fileOut == NULL) return false;

    fclose(fileOut);

    return true;
}

bool add_item_in_file(Stock data){
    char dir[200];

    strcat(strcat(strcpy(dir,"data/storage/"),this_filename),".bin");

    fileOut = fopen(dir, "ab");

    if(fileOut == NULL) return false;

    if(fwrite(&data, sizeof(Stock),1,fileOut) != 1) return false;

    if(fclose(fileOut) == EOF) return false;

    return true;
}

void on_stock_enter(GtkWidget *widget,gpointer* St){
    read_ctrl_names();
    get_ctrl_names();

    estoq_nome = GTK_LABEL(gtk_builder_get_object(builder,"nome_do_estoque"));
    estoq_desc = GTK_LABEL(gtk_builder_get_object(builder,"descricao_do_estoque"));
    estoq_tipo = GTK_LABEL(gtk_builder_get_object(builder,"tipo_do_estoque"));
    estoq_limite = GTK_LABEL(gtk_builder_get_object(builder,"qnt_limite"));

    gtk_label_set_text(estoq_nome,(char *)St);
    strcpy(this_filename,(char *)St);
    g_print("\nEstoque atual:%s",this_filename);

    for(int j = 0; j < ctrl_names ; j++){
        if(strcmp(ctrl_stNames[j].nome,(char *)St) == 0){
        
        gtk_label_set_text(estoq_desc   ,(char *)ctrl_stNames[j].desc);
        gtk_label_set_text(estoq_tipo   ,(char *)ctrl_stNames[j].tipo);
        gtk_label_set_text(estoq_limite ,(char *)ctrl_stNames[j].quant);
        
        }
    }

    
    gtk_list_store_clear(store_model);
    gtk_stack_set_visible_child_name(stack, "view_estoque");        
}

void on_estoq_btn_voltar_clicked(GtkWidget *widget, gpointer data){
    gtk_stack_set_visible_child_name(stack, "view_menu");        
}

void on_estoq_btn_adicionar_clicked(GtkWidget *widget, gpointer data){
    gtk_stack_set_visible_child_name(stack,"view_cadastro_item");        
}

void on_cad_item_btn_confirm_clicked(GtkWidget *widget, gpointer data){
    const char *item_nome =  gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object (builder, "cad_item_nome")));
    const char *item_tipo =  gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object (builder, "cad_item_tipo")));
    const char *item_desc = gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object  (builder, "cad_item_descricao")));
    const char *item_valor =  gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "cad_item_valor")));
    const char *item_quant = gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object (builder, "cad_item_quantidade")));

    criar_item(item_nome,item_tipo,item_desc,item_valor,item_quant);
}

void criar_item(const char  *item_nome,const char  *item_tipo,const char  *item_desc,const char *item_quantidade,const char *item_valor){
    Stock this;

    strcpy(this.nome   ,item_nome);
    strcpy(this.tipo   ,item_tipo);
    strcpy(this.desc   ,item_desc);
    strcpy(this.quant  ,item_quantidade);
    strcpy(this.valor  ,item_valor);

   
    if(add_item_in_file(this) == true){
            g_print( "\nEstoque atualizado com sucesso!"); 
    
            mensagem("Item adicionado com sucesso!","Atualize o seu estoque para visualiza-lo","pac_sucess","emblem-default");
    }

    else{
        mensagem("Falha ao adicionar item","Verifique se os campos estão preenchidos corretamente","pac_404","dialog-warning");
    }
    
    gtk_stack_set_visible_child_name(stack, "view_estoque");
}

void on_estoq_btn_atualizar_clicked(GtkWidget *widget, gpointer data){
    int id;
    GtkTreeIter iter;
    get_stock_size();
    read_stock_itens();    
    
    gtk_list_store_clear(store_model);

    for(int j = 0 ; j < stock_size ; j++){
        this_Stock[j].id = j + 1;

        gtk_list_store_append(store_model,&iter);
        gtk_list_store_set(store_model,&iter,
                            0, this_Stock[j].id,
                            1, this_Stock[j].nome,
                            2, this_Stock[j].tipo,
                            3, this_Stock[j].desc,
                            4, this_Stock[j].quant,
                            5, this_Stock[j].valor,
                            -1);
    }
}



void on_edit_item_btn_voltar_clicked(GtkWidget *widget, gpointer data){
    gtk_stack_set_visible_child_name(stack, "view_estoque");        

}

void on_cad_item_btn_voltar_clicked(GtkWidget *widget, gpointer data){
    gtk_stack_set_visible_child_name(stack, "view_estoque");
}


void on_estoq_btn_editar_clicked(GtkWidget *widget, gpointer data){
    popup_edit = GTK_WIDGET(gtk_builder_get_object(builder,"pop_up_edit"));
    gtk_widget_show(popup_edit);
}

void on_popup_edit_btn_confirm_clicked(GtkWidget *widget, gpointer data){
    const char *item_id =  gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object (builder, "entry_item_edit")));
    this_id = atoi(item_id);

    if(verificar_id(this_id) == false){
     mensagem("Falha ao encontrar o item","O id digitado não existe,digite um id correto!","pac_404","dialog-warning");  
    }
    else{
    gtk_stack_set_visible_child_name(stack, "view_edit_item");
    gtk_widget_hide(popup_edit);
    }

}

void on_edit_item_btn_confirm_clicked(GtkWidget *widget, gpointer data){
    char dir[200];
    Stock edit_Stock;
    int id = this_id -1;

    strcat(strcat(strcpy(dir,"data/storage/"),this_filename),".bin");
    const char *item_nome =  gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object (builder, "edit_item_nome")));
    const char *item_tipo =  gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object (builder, "edit_item_tipo")));
    const char *item_desc = gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object  (builder, "edit_item_descricao")));
    const char *item_valor =  gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "edit_item_valor")));
    const char *item_quant = gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object (builder, "edit_item_quantidade")));

    read_stock_itens();

    strcpy(edit_Stock.nome,item_nome);
    strcpy(edit_Stock.tipo,item_tipo);
    strcpy(edit_Stock.desc,item_desc);
    strcpy(edit_Stock.valor,item_valor);
    strcpy(edit_Stock.quant,item_quant);


    fileOut = fopen(dir, "rb+");

    fseek(fileOut,id*sizeof(Stock),SEEK_SET);
    fwrite(&edit_Stock,sizeof(Stock),1,fileOut);
    fseek(fileIn,0L,SEEK_SET);

    fclose(fileOut);

    gtk_stack_set_visible_child_name(stack, "view_estoque");        
}

bool verificar_id(int id){
    int j = 0;
    bool search = false;

    get_stock_size();
    
    while(j < stock_size){
        if(id == this_Stock[j].id){
            return true;
        }
        j++;
    }
    
    return false;
}

void on_popup_edit_btn_voltar_clicked(GtkWidget *widget, gpointer data){
    gtk_widget_hide(popup_edit);
}

void on_estoq_btn_excluir_clicked(GtkWidget *widget, gpointer data){
    popup_delete = GTK_WIDGET(gtk_builder_get_object(builder,"pop_up_delete"));
    gtk_widget_show(popup_delete);
}

void on_popup_del_btn_confirm_clicked(GtkWidget *widget, gpointer data){
    const char *item_id =  gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object (builder, "entry_item_delete")));
    this_id = atoi(item_id);


    if(verificar_id(this_id) == false){
     mensagem("Falha ao encontrar o item","O id digitado não existe,digite um id correto!","pac_404","dialog-warning");  
    }

    else
    {
    int j = 0;
    char dir[150];
    
    get_stock_size();
    int new_stock_size = stock_size-1;
    Stock stock_temp;

    strcat(strcat(strcpy(dir,"data/storage/"),this_filename),".bin");
    remove(dir);

    fileIn = fopen(dir,"ab");
    while(j < stock_size){
    if(this_Stock[j].id != this_id){
    strcpy(stock_temp.nome   ,this_Stock[j].nome );
    strcpy(stock_temp.tipo   ,this_Stock[j].tipo );
    strcpy(stock_temp.desc   ,this_Stock[j].desc );
    strcpy(stock_temp.quant  ,this_Stock[j].quant );
    strcpy(stock_temp.valor  ,this_Stock[j].valor );
    
    add_item_in_file(stock_temp);    
    }
      j++;
   }
    
    fclose(fileIn);

    gtk_stack_set_visible_child_name(stack, "view_estoque");
    gtk_widget_hide(popup_delete);
    }
}

void on_popup_del_btn_voltar_clicked(GtkWidget *widget, gpointer data){
    gtk_widget_hide(popup_delete);
}

