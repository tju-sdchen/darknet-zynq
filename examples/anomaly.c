#include "darknet.h"

void train_anomaly(char *cfgfile, char *filename, char *weightfile)
{
    srand(23);
    float avg_loss = -1;
    char *base = basecfg(cfgfile);
    printf("%s\n", base);
    network *net = load_network(cfgfile, weightfile, 0);
    printf("Learning Rate: %g, Momentum: %g, Decay: %g, ADAM: %d\n", net->learning_rate, 
        net->momentum, net->decay, net->adam);

    char *backup_directory = "./backup/";
    int classes = 32;
    int N = 984;

    int epoch = (*net->seen)/N;
    data train = load_rf_data(filename);
    
    float *input = malloc(sizeof(float)*984*32);
    float *targets = malloc(sizeof(float)*984*32);
    
    while(get_current_batch(net) < net->max_batches || net->max_batches == 0){
        clock_t time=clock();

        float loss = train_network(net, train);
        if(avg_loss == -1) avg_loss = loss;
        avg_loss = avg_loss*.95 + loss*.05;

        epoch = *net->seen/N;

        if(epoch%100 == 0){
            printf("%ld: %f, %f avg, %f rate, %lf seconds, %ld windows\n", get_current_batch(net), loss, avg_loss, get_current_rate(net), sec(clock()-time), *net->seen);
        }
        if(epoch%1000 == 0){
            char buff[256];
            sprintf(buff, "%s/%s_%d.weights",backup_directory,base, epoch);
            save_weights(net, buff);
        }
        if(get_current_batch(net)%5000 == 0){
            char buff[256];
            sprintf(buff, "%s/%s.backup",backup_directory,base);
            save_weights(net, buff);
        }

        

    }
    char buff[256];
    sprintf(buff, "%s/%s.weights", backup_directory, base);
    save_weights(net, buff);
    

    free_network(net);
    free(base);
    free_data(train);
}


void test_anomaly(char *cfgfile, char *filename, char *weightfile)
{

    //network *net = load_network(cfgfile, weightfile, 0);
    //srand(time(0));

    //clock_t time;
    //float avg_acc = 0;
    //float avg_top5 = 0;
    //data test = load_cifar10_data("data/cifar/cifar-10-batches-bin/test_batch.bin");

    //time=clock();

    //float *acc = network_accuracies(net, test, 2);
    //avg_acc += acc[0];
    //avg_top5 += acc[1];
    //printf("top1: %f, %lf seconds, %d images\n", avg_acc, sec(clock()-time), test.X.rows);
    //free_data(test);
}

void run_anomaly(int argc, char **argv)
{
    if(argc < 4){
        fprintf(stderr, "usage: %s %s [train/test] [cfg] [filename] [weights (optional)]\n", argv[0], argv[1]);
        return;
    }

    char *cfg = argv[3];
    char *filename = argv[4];
    char *weights = (argc > 5) ? argv[5] : 0;
    if(0==strcmp(argv[2], "train")) train_anomaly(cfg, filename, weights);
    else if(0==strcmp(argv[2], "test")) test_anomaly(cfg, filename, weights);
}


