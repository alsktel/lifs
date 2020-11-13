# LIFS
List File System is based on doubly linked lists. LIFS supports partitioning
and each partition is a node of the list. LIFS contains files (LIFS units),
directory is a file that doesn't have data but data pointer points to first
file in this directory.

**NOTICE!** First partition sector must be within first 255 disk sectors, lower byte of its sector id is stored in 4th byte of MBR.

**Sector ids:** global sector id - id of sector on **disk**, local - on current **partition**.


## Partitions
Each partition has its own header. The header contains following fields
(each field is 32 bits):

|     Field     |                   Description                             |
|---------------|-----------------------------------------------------------|
|   Signature   |   LIFS signature is always 0x5346494C ("LIFS")            |
|    Version    |   LIFS version. Actual is 1.0                             |
|    BS data    |   Reserved for [SXB](https://github.com/eastev/sxb) data  |
|     Label     |   LIFS has 3 types of labels: BOOT, ROOT, DATA            |
|    Sector     |   Global id of this header disk sector                    |
|     Size      |   LIFS size in sectors (sector is 512 bytes)              |
|     Flags     |   Partition flags, see [flags](#partition-flags)          |
|    Content    |   Local id of sector with first file on this partition    |
|  Bitmap start |   Local id of first sector with [bitmap](#bitmap)         |
|  Bitmap size  |   Size of [bitmap](#bitmap) in sectors                    |
|   UID higher  |   UID higher 32 bits                                      |
|   UID lower   |   UID lower 32 bits                                       |
|    System     |   Reserved for [TSIX](https://github.com/eastev/fsix) data|
|     Mount     |   Global id of sector with this partition mount directory |
|   Previous|Global id of sector with previous partition header (if 0 - first)|
|     Next      |Global id of sector with next partition header (if 0 - last)|
| Reserved x 448|   Reserved for aligning to sector size                    |


### Partition flags
|   Flag    |                       Description                             |
|-----------|---------------------------------------------------------------|
|   boot    |   Whether this is boot partition                              |
|   root    |   Whether this is root partition |
|   data    |   Whether this is data partition |
| protected |   Whether this partition is protected from writing |
|  mounted  |   Whether this partition is mounted |
| reserved  |   Reserved for aligning to 32 bits |

### Bitmap
Bitmap is located after partition header and shows which sectors are used and 
which are free. Each bit of this bitmap is a flag that shows sector state
(1 - used, 0 - free).


## LIFS units
LIFS units are files and directories. Each unit
has header and content sectors (for file - file data, 
for directory - files). All units form hierarchical lists:
directories and files located on same level form list,
directories and files located 1 level below or above forms their
own lists.

### Unit header
|     Field     |                       Description                         |
|---------------|-----------------------------------------------------------|
|     Name      |   256 chars name of this unit in UTF-8                    |
|     Link      |   Real unit that linked here (Global id of header sector) |
|     Size      |   Unit size (file - size sectors, directory - files count)|
|     Flags     |   Unit [flags](#unit-flags)   |
|    Content    |   Local id of sector with content (see [this](#lifs-units))|
|    Sector     |   Local id of this header sector|
|    Parent     |   Local id of this unit parent (if 0 - in root of LIFS)|
|   Previous    |Local id of sector with previous unit header (if 0 - first)|
|     Next      |   Local id of sector with next unit header (if 0 - last)|
|   Additional  | Reserved for additional data or system-specified data |
|    Created    |Time when this unit was created (in seconds since 01.01.2020)|
|    Edited|Time when this unit was last edited (in seconds since 01.01.2020)|
|Accessed|Time when this unit was last accessed (in seconds since 01.01.2020)|
|Reserved| Reserved for aligning to 512 bytes|

### Unit flags
|   Flag    |           Description     |
|-----------|---------------------------|
|   dir     | Whether this unit is a directory|
|   link    | Whether this unit is a link entry of other unit|
|   boot| Whether this file is a part of [SXB](https://github.com/eastev/sxb)|
|   dev     | Whether this file is a device entry |
| encrypted | Whether this file is encrypted |
|  accessed | Whether this file is accessed |
|  u_read   | Whether current user can read this file |
|  u_write   | Whether current user can write to this file |
|  u_exec   | Whether current user can execute this file |
|  g_read   | Whether current user's group can read this file |
|  g_write   | Whether current user's group can write to this file |
|  g_exec  | Whether current user's group can execute this file |
|  o_read   | Whether other groups and users can read this file |
|  o_write   | Whether other groups and users can write to this file |
|  o_exec  | Whether other groups and users can execute this file |
| reserved  | reserved for aligning to 32 bits |

### File data sectors (nodes)
Each sector with with file data has pointers to previous and next sectors in data list. Data is a 504 bytes between this 2 pointers.

|   Bytes   |                          Description                          |
|-----------|---------------------------------------------------------------|
| 4 bytes| Pointer to previous node (if 0 - first node)
| 504 bytes | Data |
| 4 bytes | Pointer to next node (if 0 - last node)

**NOTICE!** End of file data (**FDE byte**) is a byte with value **0x1C**.

## Developing of LIFS
Current version of LIFS is 1.1. Further plans of developing is adding more flags during developing of [42IX](https://github.com/eastev/42ix).

You can develop and use it in terms of GNU General Public License v3.0, 
**but you must save first 64 bits of LIFS header**.
