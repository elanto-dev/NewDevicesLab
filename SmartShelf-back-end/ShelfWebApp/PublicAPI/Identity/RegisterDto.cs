using System.ComponentModel.DataAnnotations;

namespace ShelfWebApp.PublicAPI.Identity
{
    public class RegisterDto
    {
        public string UserName { get; set; }

        [Required]
        public string Email { get; set; }

        [Required]
        [MinLength(6)]
        public string Password { get; set; }
    }
}
