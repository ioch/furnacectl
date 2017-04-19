/* 
 * File:   menuconfig.h
 * Author: miceuz
 *
 * Created on January 8, 2015, 3:51 PM
 */

#ifndef MENUCONFIG_H
#define	MENUCONFIG_H

	/** Configuration for the desired memory storage location of created menu items.
	 *  This may be configured to locate the menus into FLASH memory space, SRAM, or
	 *  any other suitable memory space.
	 */
	#define MENU_ITEM_STORAGE              

	/** Configuration for the macro or function required to read out a pointer from
	 *  the memory storage space set by \ref MENU_ITEM_STORAGE.
	 *
	 *  \param[in] Addr  Address of the pointer to read
	 */
	#define MENU_ITEM_READ_POINTER(Addr)   (Addr)



#endif	/* MENUCONFIG_H */

