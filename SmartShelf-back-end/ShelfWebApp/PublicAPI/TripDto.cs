using System;
using System.Collections.Generic;

namespace ShelfWebApp.PublicAPI
{
    public class TripDto
    {
        public int Id { get; set; }
        public int UserId { get; set; }
        public string Location { get; set; }
        public DateTime StartDateTime { get; set; }
        public DateTime EndDateTime { get; set; }
        public int? ReminderSeconds { get; set; }
        public List<ShelvesSetupDto> ShelfSetups { get; set; }
    }

    public class ShelvesSetupDto
    {
        public int ShelfId { get; set; }
        public string Identifier { get; set; }
        public List<CategoriesSetupDto> CategoriesOnShelf { get; set; }

    }

    public class CategoriesSetupDto
    {
        public int CategoryId { get; set; }
        public string Name { get; set; }
        public int Index { get; set; }
        public bool ItemRemoved { get; set; }
    }
}
