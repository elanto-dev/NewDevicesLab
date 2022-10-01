using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace ShelfWebApp.PublicAPI
{
    public class ShelfDto
    {
        public string Identifier { get; set; }
        public bool Main { get; set; } = true;
        public int? MainShelfId { get; set; }
        public string Location { get; set; }
    }
}
