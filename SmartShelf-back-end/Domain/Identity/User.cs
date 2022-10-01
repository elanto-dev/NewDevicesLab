
using System.Collections.Generic;
using Microsoft.AspNetCore.Identity;

namespace Domain.Identity
{
    public class User : IdentityUser<int>
    {
        public override int Id { get; set; }
        public List<Shelf> Shelves { get; set; }
        public List<Trip> Trips { get; set; }
    }
}
