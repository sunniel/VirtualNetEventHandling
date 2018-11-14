The OldInventoryCreation class create the old content inventory.

The NewInventoryCreation class create the new content inventory with the specified number of file modification.

The ContentIntegrityCheckerWithFileChangeNum class run the content integrity check after the file modifications

In the application.properties, 
1. The "vn.content.storage.content" property specifies the location of user contents.
2. The "vn.content.storage.inventory.old" property specifies the location where the old content inventory will be generated.
3. The "vn.content.storage.inventory.new" property specifies the location where the new content inventory will be generated.
4. the "vn.content.storage.content.change" property specify the number of file modification in generating the new content inventory

