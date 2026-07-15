#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "poker_dev"
#define BUFFER_SIZE 256

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gabriel - Pedro - UNICAMP");
MODULE_DESCRIPTION("Pseudo Device Driver para o Lab 6");

static int major_number;
static char kernel_buffer[BUFFER_SIZE] = "Buffer Inicial do Kernel Linux!";
static short size_of_message = 31;

// Função chamada quando o espaço de usuário lê o arquivo (/dev/poker_dev)
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    int errors = 0;
    
    // Se o offset já leu tudo, retorna 0 (EOF)
    if (*offset > 0) return 0;

    // Transfere dados do espaço de kernel para o espaço de usuário de forma segura
    errors = copy_to_user(buffer, kernel_buffer, size_of_message);

    if (errors == 0) {
        printk(KERN_INFO "PokerDriver: Mensagem enviada com sucesso para o usuário\n");
        *offset += size_of_message;
        return size_of_message;
    } else {
        printk(KERN_INFO "PokerDriver: Falha ao enviar %d caracteres para o usuário\n", errors);
        return -EFAULT;
    }
}

// Função chamada quando o espaço de usuário escreve no arquivo (ex: echo "algo" > /dev/poker_dev)
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
    if (len > BUFFER_SIZE) len = BUFFER_SIZE;

    // Transfere dados do espaço de usuário para o espaço de kernel
    if (copy_from_user(kernel_buffer, buffer, len)) {
        return -EFAULT;
    }

    kernel_buffer[len] = '\0'; // Garante o caractere nulo de fim de string
    size_of_message = len;
    printk(KERN_INFO "PokerDriver: Recebido %zu caracteres do usuário\n", len);
    return len;
}

// Estrutura mapeando as operações de arquivo do Linux para as nossas funções
static struct file_operations fops = {
    .read = dev_read,
    .write = dev_write,
};

// Função executada quando o driver é carregado (insmod)
static int __init poker_init(void) {
    printk(KERN_INFO "PokerDriver: Inicializando o modulo\n");
    
    // Aloca dinamicamente um Major Number para o nosso Character Device
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "PokerDriver: Falha ao registrar um major number\n");
        return major_number;
    }
    printk(KERN_INFO "PokerDriver: Registrado com sucesso! Major Number: %d\n", major_number);
    printk(KERN_INFO "PokerDriver: Crie o nó usando: mknod /dev/%s c %d 0\n", DEVICE_NAME, major_number);
    return 0;
}

// Função executada quando o driver é removido (rmmod)
static void __exit poker_exit(void) {
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "PokerDriver: Modulo removido com sucesso.\n");
}

module_init(poker_init);
module_exit(poker_exit);
