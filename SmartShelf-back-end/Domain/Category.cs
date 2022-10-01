using System;
using System.Collections.Generic;

namespace Domain
{
    public class Category
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public string PicturePath { get; set; }
        public List<ShelfSetupCategories> ShelfSetupCategoriesList { get; set; }
    }
}
