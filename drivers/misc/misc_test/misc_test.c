#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/sched.h>

static char kbuf[1024] = {0};
static ssize_t show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    char info[]="None\n";
    return scnprintf(buf,sizeof(info),info);
}

static ssize_t store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    // printk("%s is called\n",__func__);
    // strncpy(kbuf,buf,count);
    // printk("user_buf:%s,count:%ld|after copy,kbuf:%s\n",buf,count,kbuf);
	pid_t pid;
	pid = simple_strtoul(buf, NULL, 10);
	printk("%lu\n", pid);
	struct task_struct *task = find_task_by_vpid(pid);
	if (!task) {
		pr_err("No such process.");
		return count;
	}
	set_task_state(task, TASK_UNINTERRUPTIBLE);
    return count;
}

//show是name，就是sys中的文件名
static struct kobj_attribute misc_test_sysfs =__ATTR(processd, S_IRUSR |  S_IWUSR, show, store);

// static struct kobj_attribute misc_test_sysfs =__ATTR(write, S_IWUSR, NULL,my_store);

static struct attribute *misc_test_sysfs_attr[] = {
    &misc_test_sysfs.attr,
    // &misc_test_sysfs_write.attr,
    NULL,
};

static struct attribute_group misc_test_sysfs_attr_group = {
    // .name = "sub_my_attr",      //不写这个成员就不会创建子文件夹
    .attrs = misc_test_sysfs_attr,
};

static struct kobject *misc_test_kobj = NULL;
static int misc_test_sys_init(void)
{
    misc_test_kobj = kobject_create_and_add("misc_test", NULL);
    sysfs_create_group(misc_test_kobj, &misc_test_sysfs_attr_group);
}

static int __init misc_test_init(void)
{
	if (misc_test_sys_init()) {
		pr_err("misc_test_sys_init error");
	}
    return 0;
} 
module_init(misc_test_init);
