#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x8cbcb738, "module_layout" },
	{ 0x8666fd6f, "device_remove_file" },
	{ 0xc53caab2, "kmalloc_caches" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0x803b997d, "usb_altnum_to_altsetting" },
	{ 0xd0d8621b, "strlen" },
	{ 0x38390f13, "dev_set_drvdata" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0x7ab88a45, "system_freezing_cnt" },
	{ 0xcef4802e, "usb_kill_urb" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0x39fcde33, "mutex_unlock" },
	{ 0x999e8297, "vfree" },
	{ 0x91715312, "sprintf" },
	{ 0xb277bac, "kthread_create_on_node" },
	{ 0x733c3b54, "kasprintf" },
	{ 0xd2aaca06, "input_event" },
	{ 0x68dfc59f, "__init_waitqueue_head" },
	{ 0xb2d307de, "param_ops_short" },
	{ 0xf97456ea, "_raw_spin_unlock_irqrestore" },
	{ 0x7e57653e, "current_task" },
	{ 0xd6618151, "freezing_slow_path" },
	{ 0xa2d13107, "usb_deregister" },
	{ 0xb6d1d311, "__mutex_init" },
	{ 0x50eedeb8, "printk" },
	{ 0x788f0778, "kthread_stop" },
	{ 0x4ea56f9, "_kstrtol" },
	{ 0x75ea440, "usb_set_interface" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0x2f287f0d, "copy_to_user" },
	{ 0xb4390f9a, "mcount" },
	{ 0x1b35a9e0, "usb_control_msg" },
	{ 0x73e20c1c, "strlcpy" },
	{ 0x16305289, "warn_slowpath_null" },
	{ 0x50bd48ba, "mutex_lock" },
	{ 0x203a7bf1, "vm_insert_page" },
	{ 0xb582b96d, "device_create_file" },
	{ 0xe54e1665, "usb_submit_urb" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x3bd1b1f6, "msecs_to_jiffies" },
	{ 0xbfe59ff4, "input_register_device" },
	{ 0xd62c833f, "schedule_timeout" },
	{ 0x4292364c, "schedule" },
	{ 0x54a9db5f, "_kstrtoul" },
	{ 0x7afc594b, "input_free_device" },
	{ 0x4482cdb, "__refrigerator" },
	{ 0xa0b04675, "vmalloc_32" },
	{ 0xe8393b16, "wake_up_process" },
	{ 0xb464fc58, "kmem_cache_alloc_trace" },
	{ 0x21fb443e, "_raw_spin_lock_irqsave" },
	{ 0xadb5559d, "param_ops_byte" },
	{ 0xe45f60d8, "__wake_up" },
	{ 0xd2965f6f, "kthread_should_stop" },
	{ 0x4f68e5c9, "do_gettimeofday" },
	{ 0x37a0cba, "kfree" },
	{ 0x2e60bace, "memcpy" },
	{ 0xea561c43, "input_unregister_device" },
	{ 0x622fa02a, "prepare_to_wait" },
	{ 0x9e61bb05, "set_freezable" },
	{ 0x7eca1f0d, "usb_register_driver" },
	{ 0x75bb675a, "finish_wait" },
	{ 0xb81960ca, "snprintf" },
	{ 0x8235805b, "memmove" },
	{ 0x498897e, "vmalloc_to_page" },
	{ 0x487d9343, "param_ops_ushort" },
	{ 0xb570d970, "dev_get_drvdata" },
	{ 0xb91f013f, "usb_free_urb" },
	{ 0x9e7d6bd0, "__udelay" },
	{ 0xe4cdc925, "usb_alloc_urb" },
	{ 0xe914e41e, "strcpy" },
	{ 0x13598d26, "input_allocate_device" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("usb:v0C45p6240d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p6242d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p6243d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p6248d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p624Bd*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p624Cd*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p624Ed*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p624Fd*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p6251d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p6253d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p6260d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p6262d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p6270d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p627Ad*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p627Bd*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p627Cd*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p627Fd*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p6280d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p6282d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p6283d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p6288d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p628Ad*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p628Bd*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p628Cd*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p628Ed*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p628Fd*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p62A0d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p62A2d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p62B0d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p62B3d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p62BAd*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p62BBd*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p62BCd*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0C45p62BEd*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v045Ep00F4d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v145Fp013Dd*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v04F2pA128d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:v0458p7029d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:vA168p0610d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:vA168p0611d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:vA168p0613d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:vA168p0618d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:vA168p0614d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:vA168p0615d*dc*dsc*dp*icFFisc*ip*in*");
MODULE_ALIAS("usb:vA168p0617d*dc*dsc*dp*icFFisc*ip*in*");

MODULE_INFO(srcversion, "7FC17D8D7CCFF654413A3AC");
