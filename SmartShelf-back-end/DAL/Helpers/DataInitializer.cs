using System.Collections.Generic;
using System.Linq;
using Domain;
using Microsoft.EntityFrameworkCore;

namespace DAL.Helpers
{
    public class DataInitializer
    {
        public static void MigrateDatabase(AppDbContext context)
        {
            context.Database.Migrate();
        }
        public static bool DeleteDatabase(AppDbContext context)
        {
            return context.Database.EnsureDeleted();
        }

        public static void SeedData(AppDbContext context, Dictionary<string, string> categoriesNamesAndPics)
        {
            foreach (var categoryNameAndPic in categoriesNamesAndPics)
            {
                var name = categoryNameAndPic.Key;
                var picturePath = categoryNameAndPic.Value;
                var dbCategory = context.Categories.FirstOrDefault(c => c.Name == name);
                if (dbCategory == null)
                {
                    var category = new Category {Name = name, PicturePath = picturePath };
                    context.Categories.Add(category);
                }
                else
                {
                    dbCategory.Name = name;
                    dbCategory.PicturePath = picturePath;
                    context.Categories.Update(dbCategory);
                }
            }
            context.SaveChanges();
        }
    }
}